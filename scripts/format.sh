#!/usr/bin/env sh
set -eu

repo_root="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"

find "$repo_root" \
  \( -path "$repo_root/.git" \
  -o -path "$repo_root/.idea" \
  -o -path "$repo_root/.claude" \
  -o -path "$repo_root/build" \
  -o -path "$repo_root"/cmake-build-\* \) -prune \
  -o \( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \
  -o -name '*.h' -o -name '*.hh' -o -name '*.hpp' -o -name '*.hxx' \) \
  -type f -exec clang-format -i {} +
