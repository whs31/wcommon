#!/usr/bin/env python3
"""Formats C/C++ code in place with the clang-format version pinned in pyproject.toml."""

import argparse
import hashlib
import os
import subprocess
from pathlib import Path

from clang_tools import (
  REPO_ROOT,
  Tool,
  ToolError,
  batched,
  console,
  error_line,
  file_link,
  find_format_sources,
  info_line,
  ok_line,
  pinned_tool,
  plural,
  relative,
  run_parallel,
)
from rich.text import Text


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
  parser = argparse.ArgumentParser(
    description="Format C/C++ code in place with the pinned clang-format."
  )
  parser.add_argument(
    "-j",
    "--jobs",
    type=int,
    default=os.cpu_count() or 1,
    help="parallel clang-format processes (default: %(default)s)",
  )
  args = parser.parse_args(argv)
  if args.jobs < 1:
    parser.error("--jobs must be at least 1")
  return args


def digest(path: Path) -> bytes:
  return hashlib.blake2b(path.read_bytes(), digest_size=16).digest()


def format_batch(tool: Tool, batch: list[Path]) -> None:
  result = subprocess.run(
    [tool.executable, "-i", "--", *map(str, batch)],
    cwd=REPO_ROOT,
    text=True,
    capture_output=True,
    check=False,
  )
  if result.returncode != 0:
    raise ToolError(f"clang-format failed:\n{(result.stderr or result.stdout).strip()}")


def main(argv: list[str] | None = None) -> int:
  args = parse_args(argv)
  try:
    tool = pinned_tool("clang-format")
    sources = find_format_sources()
    info_line(f"{tool} · {plural(len(sources), 'file')}")

    before = {source: digest(source) for source in sources}
    run_parallel(
      batched(sources),
      args.jobs,
      "formatting",
      lambda batch: format_batch(tool, batch),
    )
    changed = [source for source in sources if digest(source) != before[source]]
  except ToolError as error:
    error_line(str(error))
    return 1
  except KeyboardInterrupt:
    error_line("formatting interrupted")
    return 130

  if not changed:
    ok_line(f"{plural(len(sources), 'file')} already formatted")
    return 0

  ok_line(f"reformatted {plural(len(changed), 'file')}")
  for source in changed:
    console.print(Text.assemble("    ", file_link(relative(source))))
  return 0


if __name__ == "__main__":
  raise SystemExit(main())
