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
    | grep -zEv '/include/radar/mms/common/(json|glaze)/' \
    | sort -z
)

# Serialization adapters are header-only and optional. Analyze them through the test translation
# units of the enabled adapters, so clang-tidy selects compile commands with their include paths.
mapfile -d '' adapter_tests < <(
  find "$repo_root/tests" -maxdepth 1 -type f \
    \( -name '*_json.cc' -o -name '*_glaze.cc' -o -name 'test_json_qt.cc' -o -name 'test_glaze_qt.cc' \) \
    -print0 | sort -z
)
for test_file in "${adapter_tests[@]}"; do
  if grep -Fq -- "$test_file" "$build_dir/compile_commands.json"; then
    source_files+=("$test_file")
  fi
done

clang-tidy \
  -p "$tidy_build_dir" \
  --quiet \
  --warnings-as-errors='*' \
  --extra-arg=-xc++ \
  "${source_files[@]}"
