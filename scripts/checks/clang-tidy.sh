#!/usr/bin/env bash
set -euo pipefail
export LC_ALL=C

repo_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)
build_dir=${1:-build/Debug}

if [[ "$build_dir" != /* ]]; then
  build_dir="$repo_root/$build_dir"
fi

if [[ ! -f "$build_dir/compile_commands.json" ]]; then
  echo "Missing compilation database: $build_dir/compile_commands.json" >&2
  exit 1
fi

tidy_build_dir=$build_dir
if grep -Fq -- '-mno-direct-extern-access' "$build_dir/compile_commands.json"; then
  tidy_build_dir=$(mktemp -d)
  trap 'rm -rf -- "$tidy_build_dir"' EXIT
  sed 's/ -mno-direct-extern-access//g' \
    "$build_dir/compile_commands.json" > "$tidy_build_dir/compile_commands.json"
fi

mapfile -d '' source_files < <(
  find "$repo_root/include" "$repo_root/src" -type f \( -name '*.h' -o -name '*.cc' \) -print0 \
    | grep -zv '/include/radar/mms/common/json/' \
    | sort -z
)

# JSON adapters are header-only. Analyze them through their test translation unit so
# clang-tidy selects the compile command containing nlohmann_json's include path.
source_files+=(
  "$repo_root/tests/test_chrono_json.cc"
  "$repo_root/tests/test_geometry_json.cc"
  "$repo_root/tests/test_identifiers_json.cc"
  "$repo_root/tests/test_measurements_json.cc"
  "$repo_root/tests/test_network_json.cc"
)
if grep -Fq -- 'test_json_qt.cc' "$build_dir/compile_commands.json"; then
  source_files+=("$repo_root/tests/test_json_qt.cc")
fi

clang-tidy \
  -p "$tidy_build_dir" \
  --quiet \
  --warnings-as-errors='*' \
  --extra-arg=-xc++ \
  "${source_files[@]}"
