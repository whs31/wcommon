"""Shared helpers for the clang-format / clang-tidy scripts: pinned tools, source discovery and console output."""

import os
import sysconfig
from collections.abc import Callable
from concurrent.futures import ThreadPoolExecutor, as_completed
from dataclasses import dataclass
from importlib import metadata
from pathlib import Path

from rich.console import Console
from rich.progress import (
  BarColumn,
  MofNCompleteColumn,
  Progress,
  TextColumn,
  TimeElapsedColumn,
)
from rich.text import Text

REPO_ROOT = Path(__file__).resolve().parent.parent
SOURCE_SUFFIXES = {".c", ".cc", ".h"}
EXCLUDED_DIRS = {
  ".git",
  ".idea",
  ".claude",
  ".agents",
  ".venv",
  "build",
}
EXCLUDED_DIR_PREFIXES = ("cmake-build-",)
CLANG_FORMAT_BATCH = 64

console = Console(highlight=False, soft_wrap=True)
err_console = Console(stderr=True, highlight=False, soft_wrap=True)


class ToolError(RuntimeError):
  """An expected failure with a user-facing message."""


@dataclass(frozen=True)
class Tool:
  name: str
  executable: str
  version: str

  def __str__(self) -> str:
    return f"{self.name} {self.version}"


def pinned_tool(name: str) -> Tool:
  """Resolves a tool pinned in pyproject.toml, never falling back to whatever is in PATH."""
  suffix = ".exe" if os.name == "nt" else ""
  executable = Path(sysconfig.get_path("scripts")) / f"{name}{suffix}"
  try:
    version = metadata.version(name)
  except metadata.PackageNotFoundError:
    version = None
  if version is None or not executable.is_file():
    raise ToolError(
      f"pinned {name} is not installed in this environment; "
      "run through 'uv run' (just fmt / just check) or 'uv sync'"
    )
  return Tool(name, str(executable), version)


def find_files(matches: Callable[[str], bool]) -> list[Path]:
  """Lists repository files whose name matches, skipping VCS, tooling and build directories."""
  found = []
  for root, dirs, files in os.walk(REPO_ROOT):
    if Path(root) == REPO_ROOT:
      dirs[:] = [
        name
        for name in dirs
        if name not in EXCLUDED_DIRS and not name.startswith(EXCLUDED_DIR_PREFIXES)
      ]
    found.extend(Path(root) / name for name in files if matches(name))
  return sorted(found)


def find_format_sources() -> list[Path]:
  return find_files(lambda name: Path(name).suffix in SOURCE_SUFFIXES)


def batched[T](items: list[T], size: int = CLANG_FORMAT_BATCH) -> list[list[T]]:
  return [items[i : i + size] for i in range(0, len(items), size)]


def relative(path: str | Path) -> str:
  resolved = Path(path).resolve()
  try:
    return resolved.relative_to(REPO_ROOT).as_posix()
  except ValueError:
    return str(resolved)


def plural(count: int, word: str) -> str:
  return f"{count} {word}{'' if count == 1 else 's'}"


def stage_header(title: str) -> None:
  console.print()
  console.rule(Text(title, style="bold"), align="left", characters="─")


def info_line(message: str) -> None:
  console.print(Text(f"  {message}", style="dim"))


def ok_line(message: str) -> None:
  console.print(Text.assemble(("  ✓ ", "bold green"), message))


def fail_line(message: str, hint: str | None = None) -> None:
  line = Text.assemble(("  ✗ ", "bold red"), message)
  if hint:
    line.append(f" — {hint}", style="yellow")
  console.print(line)


def error_line(message: str) -> None:
  err_console.print(Text.assemble(("error: ", "bold red"), message))


def file_link(path: str) -> Text:
  absolute = (REPO_ROOT / path).resolve()
  return Text(path, style=f"bold cyan link {absolute.as_uri()}")


def progress_bar() -> Progress:
  return Progress(
    TextColumn("  [progress.description]{task.description}"),
    BarColumn(),
    MofNCompleteColumn(),
    TimeElapsedColumn(),
    console=err_console,
    transient=True,
    disable=not err_console.is_terminal,
  )


def run_parallel[T, R](
  items: list[T], jobs: int, description: str, worker: Callable[[T], R]
) -> list[R]:
  results: list[R] = []
  with progress_bar() as progress, ThreadPoolExecutor(max_workers=jobs) as pool:
    task_id = progress.add_task(description, total=len(items))
    futures = [pool.submit(worker, item) for item in items]
    for future in as_completed(futures):
      results.append(future.result())
      progress.advance(task_id)
  return results
