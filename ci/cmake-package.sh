#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: cmake-package.sh <build-dir>" >&2
  exit 2
fi

build_dir="$(cd "$1" && pwd)"
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
install_dir="$build_dir/package-smoke/install"
build_type="$(sed -n 's/^CMAKE_BUILD_TYPE:STRING=//p' "$build_dir/CMakeCache.txt")"

if [[ -z "$build_type" ]]; then
  echo "Missing CMAKE_BUILD_TYPE in $build_dir/CMakeCache.txt" >&2
  exit 2
fi

cmake -E remove_directory "$build_dir/package-smoke"
cmake --install "$build_dir" --prefix "$install_dir"

cache_option() {
  sed -n "s/^$1:BOOL=//p" "$build_dir/CMakeCache.txt"
}

is_on() {
  [[ "${1^^}" =~ ^(ON|TRUE|YES|Y|1)$ ]]
}

components=(core)
if is_on "$(cache_option RCOMMON_WITH_NLOHMANN)"; then
  components+=(json)
fi
if is_on "$(cache_option RCOMMON_WITH_GLAZE)"; then
  components+=(glaze)
fi

# Adapter headers are installed only together with their component.
for adapter in json glaze; do
  if [[ " ${components[*]} " != *" $adapter "* ]] \
    && [[ -d "$install_dir/include/radar/mms/common/$adapter" ]]; then
    echo "Headers of the disabled $adapter component were installed" >&2
    exit 1
  fi
done

for component in "${components[@]}"; do
  consumer_build="$build_dir/package-smoke/$component"
  cmake \
    -S "$repo_root/tests/cmake_consumer" \
    -B "$consumer_build" \
    -G Ninja \
    -DRCOMMON_CONSUMER_COMPONENT="$component" \
    -DCMAKE_BUILD_TYPE="$build_type" \
    -DCMAKE_PREFIX_PATH="$install_dir;$build_dir/generators"
  cmake --build "$consumer_build"
  "$consumer_build/rcommon_consumer"
done
