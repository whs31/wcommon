#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
CONANFILE="$ROOT_DIR/conanfile.py"
CMAKEFILE="$ROOT_DIR/CMakeLists.txt"

read_conan_version() {
    grep -oP '^\s*version\s*=\s*"\K[^"]+' "$CONANFILE" | head -1
}

write_conan_version() {
    local version="$1"
    local target_file="$2"
    if [ -f "$target_file" ]; then
        sed -i "s/^\(\s*version\s*=\s*\"\)[^\"]*\"/\1${version}\"/" "$target_file"
    fi
}

read_cmake_version() {
    sed -n '/^project(rcommon$/,/^)/s/^  VERSION \([^ ]*\)$/\1/p' "$CMAKEFILE"
}

write_cmake_version() {
    local version="$1"
    sed -i '/^project(rcommon$/,/^)/s/^  VERSION [^ ]*$/  VERSION '"$version"'/' "$CMAKEFILE"
}

is_semver() {
    [[ "$1" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]
}

extract_semver() {
    if [[ "$1" =~ ^([0-9]+\.[0-9]+\.[0-9]+) ]]; then
        echo "${BASH_REMATCH[1]}"
    else
        echo ""
    fi
}

to_semver_or_default() {
    local ver="$1"
    if is_semver "$ver"; then
        echo "$ver"
    else
        local s
        s=$(extract_semver "$ver")
        [ -n "$s" ] && echo "$s" || echo "0.1.0"
    fi
}

get_reference_version() {
    local conan_semver
    conan_semver=$(to_semver_or_default "$(read_conan_version)")
    echo "$conan_semver"
}

cmd_show() {
    echo "conanfile.py     : $(read_conan_version)"
    echo "CMakeLists.txt   : $(read_cmake_version)"
}

cmd_assign() {
    local ver="$1"
    write_conan_version "$ver" "$CONANFILE"
    write_cmake_version "$ver"

    echo "Assigned: conanfile.py=$ver CMakeLists.txt=$ver"
}

cmd_increment() {
    local part="$1"
    local ref
    ref=$(get_reference_version)
    local major minor patch
    IFS='.' read -r major minor patch <<< "$ref"

    case "$part" in
        patch) patch=$(( patch + 1 )) ;;
        minor) minor=$(( minor + 1 )); patch=0 ;;
        major) major=$(( major + 1 )); minor=0; patch=0 ;;
    esac

    local new="${major}.${minor}.${patch}"

    write_conan_version "$new" "$CONANFILE"
    write_cmake_version "$new"

    echo "Bumped ($part): $ref -> $new"
}

case "${1:-}" in
    "" | "--show")   cmd_show ;;
    "-a" | "--assign")
        [ -z "${2:-}" ] && { echo "Error: --assign requires a version argument"; exit 1; }
        cmd_assign "$2"
        ;;
    "-p" | "--patch") cmd_increment patch ;;
    "-m" | "--minor") cmd_increment minor ;;
    "-M" | "--major") cmd_increment major ;;
    *)
        echo "Usage: version.sh [--show | -a <VERSION> | -p | -m | -M]"
        exit 1
        ;;
esac
