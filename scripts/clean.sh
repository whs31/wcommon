#!/usr/bin/env sh
set -eu

repo_root="$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)"

rm -rfv \
  "$repo_root/build" \
  "$repo_root"/cmake-build-* \
  "$repo_root"/cmake-visual-studio-* \
  "$repo_root/.cache" \
  "$repo_root/docs/graph.html" \
  "$repo_root/graph.html" \
  "$repo_root/compile_commands.json" \
  "$repo_root/CMakeUserPresets.json" \
  "$repo_root/conan_provider.cmake"
