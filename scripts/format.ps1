$ErrorActionPreference = "Stop"

$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$repoPrefix = $repoRoot.TrimEnd("\", "/") + [System.IO.Path]::DirectorySeparatorChar

$files = Get-ChildItem -Path $repoRoot -Recurse -File -Include "*.c", "*.cc", "*.cpp", "*.cxx", "*.h", "*.hh", "*.hpp", "*.hxx" |
    Where-Object {
        $relativePath = $_.FullName.Substring($repoPrefix.Length)
        -not (
            $relativePath -like "build\*" -or
            $relativePath -like "cmake-build-*" -or
            $relativePath -like ".git\*" -or
            $relativePath -like ".idea\*" -or
            $relativePath -like ".claude\*"
        )
    }

if ($files) {
    & clang-format -i -- $files.FullName
}
