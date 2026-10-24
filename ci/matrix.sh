#!/usr/bin/env bash
# Builds and tests every Conan configuration of the GitLab test:linux matrix locally.
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

run_configuration() {
  local with_qt="$1" with_adapters="$2" namespace="$3"
  local output_folder="build/ci-qt-${with_qt,,}-adapters-${with_adapters,,}-ns-${namespace//::/-}"
  conan build . \
    --build=missing \
    -cc core:non_interactive=True \
    -s:h build_type=Debug \
    -s:b build_type=Debug \
    -o "&:shared=True" \
    -o "&:test=True" \
    -o "&:with_qt=$with_qt" \
    -o "&:with_nlohmann=$with_adapters" \
    -o "&:with_glaze=$with_adapters" \
    -o "&:namespace=$namespace" \
    -of "$output_folder"
  ctest --test-dir "$output_folder/build/Debug" --output-on-failure
  bash ci/cmake-package.sh "$output_folder/build/Debug"
  mapfile -t libraries < <(find "$output_folder/build/Debug" -type f -name 'libwcommon.so*')
  test "${#libraries[@]}" -gt 0
  linkage="$(ldd "${libraries[0]}")"
  if [[ "$with_qt" == "True" ]]; then
    grep -Fq "libQt6Core" <<< "$linkage"
    grep -Fq "libQt6Gui" <<< "$linkage"
    grep -Fq "libQt6Network" <<< "$linkage"
  else
    ! grep -Fq "libQt6Core" <<< "$linkage"
    ! grep -Fq "libQt6Gui" <<< "$linkage"
    ! grep -Fq "libQt6Network" <<< "$linkage"
  fi
}

for with_qt in False True; do
  for with_adapters in False True; do
    run_configuration "$with_qt" "$with_adapters" whs31
  done
done
# Any other namespace builds a respelled copy of the sources.
run_configuration True True acme::util
