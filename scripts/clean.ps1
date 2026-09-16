$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")

$paths = @(
    "build",
    "cmake-build-*",
    "cmake-visual-studio-*",
    ".cache",
    "docs\graph.html",
    "graph.html",
    "compile_commands.json",
    "CMakeUserPresets.json",
    "conan_provider.cmake"
)

foreach ($path in $paths) {
    Remove-Item -Path (Join-Path $repoRoot $path) -Recurse -Force -ErrorAction SilentlyContinue
}
