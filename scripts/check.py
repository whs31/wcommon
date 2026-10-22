#!/usr/bin/env python3
"""Audits C/C++ code for formatting issues and clang-tidy warnings without modifying anything."""

import argparse
import json
import os
import re
import subprocess
import tempfile
import time
from collections import Counter, defaultdict
from collections.abc import Iterable
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

from rich.text import Text

from clang_tools import (
  REPO_ROOT,
  Tool,
  ToolError,
  batched,
  console,
  error_line,
  fail_line,
  file_link,
  find_format_sources,
  info_line,
  ok_line,
  pinned_tool,
  plural,
  relative,
  run_parallel,
  stage_header,
)

# only project sources and headers; generated code (export headers) lives in the build tree
PROJECT_DIRS = r"(include|src|tests)"
# qt adds gcc-only flags to the compile commands of gcc builds; clang rejects them
GCC_ONLY_FLAGS = {"-mno-direct-extern-access"}

DIAGNOSTIC = re.compile(
  r"^(?P<path>.+?):(?P<line>\d+):(?P<column>\d+): "
  r"(?P<severity>fatal error|error|warning|note): "
  r"(?P<message>.*?)(?: \[(?P<check>[^\]]+)\])?$"
)


class CheckError(ToolError):
  """An expected check failure with a user-facing message."""


@dataclass(frozen=True)
class Diagnostic:
  path: str
  line: int
  column: int
  severity: str
  message: str
  check: str
  details: tuple[str, ...] = field(default=(), compare=False, hash=False)


@dataclass
class TidyFailure:
  """A clang-tidy run that failed without producing a parseable diagnostic."""

  source: str
  output: str


@dataclass
class StageResult:
  name: str
  ok: bool
  summary: str
  seconds: float


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
  parser = argparse.ArgumentParser(
    description=(
      "Audit C/C++ code with clang-format and clang-tidy without modifying anything. "
      "Any finding fails the audit."
    )
  )
  parser.add_argument(
    "build_dir",
    nargs="?",
    type=Path,
    help="directory containing compile_commands.json "
    "(default: the most recently configured build/<config> directory)",
  )
  parser.add_argument(
    "-j",
    "--jobs",
    type=int,
    default=os.cpu_count() or 1,
    help="parallel clang-tidy / clang-format processes (default: %(default)s)",
  )
  parser.add_argument(
    "-v",
    "--verbose",
    action="store_true",
    help="show clang-tidy notes and source excerpts under each finding",
  )
  stages = parser.add_mutually_exclusive_group()
  stages.add_argument(
    "--format-only", action="store_true", help="run only clang-format"
  )
  stages.add_argument("--tidy-only", action="store_true", help="run only clang-tidy")
  args = parser.parse_args(argv)
  if args.jobs < 1:
    parser.error("--jobs must be at least 1")
  return args


# ---------------------------------------------------------------------------------------------------------------------
# clang-format
# ---------------------------------------------------------------------------------------------------------------------


def clang_format_batch(tool: Tool, batch: list[Path]) -> list[Diagnostic]:
  result = subprocess.run(
    [tool.executable, "--dry-run", "--Werror", "--", *map(str, batch)],
    cwd=REPO_ROOT,
    text=True,
    capture_output=True,
    check=False,
  )
  diagnostics = parse_diagnostics(result.stdout + result.stderr)
  if result.returncode != 0 and not diagnostics:
    raise CheckError(
      f"clang-format failed:\n{(result.stderr or result.stdout).strip()}"
    )
  return diagnostics


def run_clang_format(jobs: int) -> StageResult:
  stage_header("clang-format")
  started = time.monotonic()
  tool = pinned_tool("clang-format")
  sources = find_format_sources()
  info_line(f"{tool} · {plural(len(sources), 'file')}")
  results = run_parallel(
    batched(sources),
    jobs,
    "checking",
    lambda batch: clang_format_batch(tool, batch),
  )

  lines_by_file: dict[str, set[int]] = defaultdict(set)
  for diagnostic in (d for batch in results for d in batch):
    lines_by_file[diagnostic.path].add(diagnostic.line)
  elapsed = time.monotonic() - started

  if not lines_by_file:
    ok_line(f"{plural(len(sources), 'file')} formatted correctly")
    return StageResult(
      "clang-format", True, f"{plural(len(sources), 'file')} clean", elapsed
    )

  fail_line(
    f"{len(lines_by_file)} of {plural(len(sources), 'file')} need formatting",
    "run 'just fmt'",
  )
  width = max(map(len, lines_by_file))
  for path in sorted(lines_by_file):
    lines = sorted(lines_by_file[path])
    shown = ", ".join(map(str, lines[:8])) + (", …" if len(lines) > 8 else "")
    console.print(
      Text.assemble(
        "    ",
        file_link(path),
        " " * (width - len(path) + 2),
        (f"{plural(len(lines), 'line')}: {shown}", "dim"),
      )
    )
  return StageResult(
    "clang-format",
    False,
    f"{plural(len(lines_by_file), 'file')} unformatted",
    elapsed,
  )


# ---------------------------------------------------------------------------------------------------------------------
# clang-tidy
# ---------------------------------------------------------------------------------------------------------------------


def parse_diagnostics(output: str) -> list[Diagnostic]:
  """Parses clang diagnostics, attaching notes and source excerpts to the preceding finding."""
  diagnostics: list[Diagnostic] = []
  current: dict[str, Any] | None = None
  details: list[str] = []

  def flush() -> None:
    if current is not None:
      diagnostics.append(Diagnostic(**current, details=tuple(details)))

  for raw_line in output.splitlines():
    match = DIAGNOSTIC.match(raw_line)
    if match is None or match["severity"] == "note":
      if (
        current is not None
        and raw_line.strip()
        and not raw_line.endswith(" generated.")
      ):
        details.append(raw_line)
      continue
    flush()
    check = match["check"] or ""
    severity = match["severity"]
    # --warnings-as-errors promotes every warning; show what clang-tidy originally reported
    if check.endswith(",-warnings-as-errors"):
      check = check.removesuffix(",-warnings-as-errors")
      severity = "warning"
    current = {
      "path": relative(match["path"]),
      "line": int(match["line"]),
      "column": int(match["column"]),
      "severity": severity,
      "message": match["message"],
      "check": check,
    }
    details = []
  flush()
  return diagnostics


def find_compile_commands(build_dir: Path | None) -> Path:
  if build_dir is not None:
    path = build_dir.resolve() / "compile_commands.json"
    if not path.is_file():
      raise CheckError(f"{relative(path)} does not exist")
    return path

  candidates = sorted(
    (REPO_ROOT / "build").glob("*/compile_commands.json"),
    key=lambda candidate: candidate.stat().st_mtime,
    reverse=True,
  )
  if not candidates:
    raise CheckError(
      "no build/*/compile_commands.json found; configure the project first (conan build .)"
    )
  return candidates[0]


def sanitize_compile_commands(source: Path, destination: Path) -> list[str]:
  """Writes a clang-compatible copy of the compilation database and returns its project sources."""
  try:
    entries = json.loads(source.read_text(encoding="utf-8"))
  except (OSError, json.JSONDecodeError) as error:
    raise CheckError(f"could not read {relative(source)}: {error}") from error

  for entry in entries:
    if "command" in entry:
      for flag in GCC_ONLY_FLAGS:
        entry["command"] = entry["command"].replace(f" {flag}", "")
    if "arguments" in entry:
      entry["arguments"] = [
        arg for arg in entry["arguments"] if arg not in GCC_ONLY_FLAGS
      ]
  destination.write_text(json.dumps(entries), encoding="utf-8")

  pattern = re.compile(project_filter(), re.IGNORECASE if os.name == "nt" else 0)
  sources = {
    str(Path(entry["directory"], entry["file"]).resolve()) for entry in entries
  }
  return sorted(source for source in sources if pattern.match(source))


def project_filter() -> str:
  root = re.escape(str(REPO_ROOT).rstrip("/\\"))
  return rf"^{root}[/\\]{PROJECT_DIRS}[/\\]"


def clang_tidy_source(
  tool: Tool, database_dir: Path, source: str
) -> tuple[str, int, str]:
  result = subprocess.run(
    [
      tool.executable,
      "-p",
      str(database_dir),
      "--quiet",
      # any finding fails the audit, regardless of the WarningsAsErrors setting editors see in .clang-tidy
      "--warnings-as-errors=*",
      f"--header-filter={project_filter()}",
      source,
    ],
    cwd=REPO_ROOT,
    text=True,
    capture_output=True,
    check=False,
  )
  return source, result.returncode, result.stdout + result.stderr


def print_diagnostic(
  diagnostic: Diagnostic, location_width: int, verbose: bool
) -> None:
  severity_style = (
    "bold red" if diagnostic.severity in {"error", "fatal error"} else "bold yellow"
  )
  line = Text.assemble(
    "    ",
    (location(diagnostic).ljust(location_width), "dim"),
    "  ",
    (diagnostic.severity.ljust(7), severity_style),
    "  ",
    diagnostic.message,
  )
  if diagnostic.check:
    line.append(f"  [{diagnostic.check}]", style="magenta")
  console.print(line)
  if verbose:
    for detail in diagnostic.details:
      console.print(Text(f"        {detail}", style="dim"))


def location(diagnostic: Diagnostic) -> str:
  return f"{diagnostic.line}:{diagnostic.column}"


def print_check_counts(diagnostics: Iterable[Diagnostic]) -> None:
  counts = Counter(diagnostic.check or "(compiler)" for diagnostic in diagnostics)
  count_width = len(str(max(counts.values())))
  console.print(Text("  by check:", style="dim"))
  for check, count in counts.most_common():
    console.print(
      Text.assemble(
        "    ",
        (str(count).rjust(count_width), "bold"),
        "  ",
        (check, "magenta"),
      )
    )


def run_clang_tidy(build_dir: Path | None, jobs: int, verbose: bool) -> StageResult:
  stage_header("clang-tidy")
  started = time.monotonic()
  tool = pinned_tool("clang-tidy")
  compile_commands = find_compile_commands(build_dir)

  with tempfile.TemporaryDirectory(prefix="rcommon-check-") as temp_dir:
    database_dir = Path(temp_dir)
    sources = sanitize_compile_commands(
      compile_commands, database_dir / "compile_commands.json"
    )
    info_line(
      f"{tool} · {relative(compile_commands)} · "
      f"{plural(len(sources), 'source')} · {plural(jobs, 'job')}"
    )
    results = run_parallel(
      sources,
      jobs,
      "analyzing",
      lambda source: clang_tidy_source(tool, database_dir, source),
    )

  # headers are analyzed once per translation unit that includes them, so findings repeat
  diagnostics: set[Diagnostic] = set()
  failures: list[TidyFailure] = []
  for source, returncode, output in results:
    parsed = parse_diagnostics(output)
    diagnostics.update(parsed)
    if returncode != 0 and not parsed:
      failures.append(TidyFailure(relative(source), output.strip()))
  elapsed = time.monotonic() - started

  if not diagnostics and not failures:
    ok_line(f"{plural(len(sources), 'source')} analyzed, no findings")
    return StageResult(
      "clang-tidy", True, f"{plural(len(sources), 'source')} clean", elapsed
    )

  by_file: dict[str, list[Diagnostic]] = defaultdict(list)
  for diagnostic in diagnostics:
    by_file[diagnostic.path].append(diagnostic)

  if diagnostics:
    fail_line(
      f"{plural(len(diagnostics), 'finding')} in {plural(len(by_file), 'file')}"
    )
    for path in sorted(by_file):
      console.print()
      console.print(Text.assemble("  ", file_link(path)))
      findings = sorted(by_file[path], key=lambda d: (d.line, d.column, d.check))
      width = max(len(location(diagnostic)) for diagnostic in findings)
      for diagnostic in findings:
        print_diagnostic(diagnostic, width, verbose)
    console.print()
    print_check_counts(diagnostics)

  for failure in sorted(failures, key=lambda f: f.source):
    fail_line(f"clang-tidy failed on {failure.source} without reporting a diagnostic")
    for line in failure.output.splitlines()[-20:]:
      console.print(Text(f"      {line}", style="dim"))

  summary = [plural(len(diagnostics), "finding")] if diagnostics else []
  if failures:
    summary.append(f"{len(failures)} failed")
  return StageResult("clang-tidy", False, ", ".join(summary), elapsed)


# ---------------------------------------------------------------------------------------------------------------------


def print_summary(results: list[StageResult]) -> None:
  stage_header("summary")
  name_width = max(len(result.name) for result in results)
  summary_width = max(len(result.summary) for result in results)
  for result in results:
    mark = ("✓", "bold green") if result.ok else ("✗", "bold red")
    console.print(
      Text.assemble(
        "  ",
        mark,
        "  ",
        (result.name.ljust(name_width), "bold"),
        "  ",
        result.summary.ljust(summary_width),
        "  ",
        (f"{result.seconds:.1f}s", "dim"),
      )
    )
  console.print()
  if all(result.ok for result in results):
    console.print(Text("all checks passed", style="bold green"))
  else:
    console.print(Text("checks failed", style="bold red"))


def main(argv: list[str] | None = None) -> int:
  args = parse_args(argv)
  results: list[StageResult] = []
  try:
    if not args.tidy_only:
      results.append(run_clang_format(args.jobs))
    if not args.format_only:
      results.append(run_clang_tidy(args.build_dir, args.jobs, args.verbose))
  except ToolError as error:
    error_line(str(error))
    return 1
  except KeyboardInterrupt:
    error_line("check interrupted")
    return 130

  print_summary(results)
  return 0 if all(result.ok for result in results) else 1


if __name__ == "__main__":
  raise SystemExit(main())
