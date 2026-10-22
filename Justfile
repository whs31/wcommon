alias v := version
alias fmt := format

default:
    @echo "Usage: just <command> [options]"
    @just --list

[doc("Formats C/C++ code with the pinned clang-format")]
[positional-arguments]
format *ARGS:
    @uv run --project . python scripts/format.py "$@"

[doc("Audits C/C++ code with the pinned clang-format and clang-tidy; pass --help for options")]
[positional-arguments]
check *ARGS:
    @uv run --project . python scripts/check.py "$@"

[windows]
clean:
    powershell -NoProfile -ExecutionPolicy Bypass -File scripts/clean.ps1

[unix]
clean:
    sh scripts/clean.sh

[doc('Show or change the library version')]
version *ARGS='--show':
    @bash scripts/version.sh {{ ARGS }}

graph:
    conan graph info . --format=html > graph.html
    mkdir -p docs
    mv graph.html docs

[doc("Generates documentation HTML")]
doc:
    cd .doc && poxy

[unix]
[doc("Builds and tests Conan configurations with and without Qt and the serialization adapters")]
ci:
    #!/usr/bin/env bash
    set -euo pipefail
    for with_qt in False True; do
      for with_adapters in False True; do
        output_folder="build/ci-qt-${with_qt,,}-adapters-${with_adapters,,}"
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
          -of "$output_folder"
        ctest --test-dir "$output_folder/build/Debug" --output-on-failure
        bash scripts/checks/cmake-package.sh "$output_folder/build/Debug"
        mapfile -t libraries < <(find "$output_folder/build/Debug" -type f -name 'librcommon.so*')
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
      done
    done
