## RCommon

<h3 align="center">
    <img src="./docs/images/icon.png" alt=""/><br/>
    <img src="./docs/images/transparent.png" height="30" width="0px"/>
    Radar common utility library
    <img src="./docs/images/transparent.png" height="30" width="0px"/><br/>
    <img src="./docs/images/transparent.png" height="30" width="0px"/><br/>
    <img src="./docs/images/badges/cxx.svg" alt=""/><img src="./docs/images/badges/cmake.svg" alt=""/>
</h3>

#### Dependencies

- <img src="./docs/images/logos/fmt.png" alt="" height="16px" width="16px"/>[**fmt**](https://github.com/fmtlib/fmt) - modern formatting library (**MIT**)
- <img src="./docs/images/logos/spdlog.jpg" alt="" height="16px" width="16px"/>[**spdlog**](https://github.com/gabime/spdlog) - fast C++ logging library (**MIT**)
- [**nlohmann/json**](https://github.com/nlohmann/json) - optional JSON adapters exposed by the
  `radar::mms::common::json` target, enabled with `with_nlohmann=True` in Conan or
  `RCOMMON_WITH_NLOHMANN=ON` in CMake (**MIT**)
- [**glaze**](https://github.com/stephenberry/glaze) - optional serialization adapters exposed by
  the `radar::mms::common::glaze` target, enabled with `with_glaze=True` in Conan or
  `RCOMMON_WITH_GLAZE=ON` in CMake (**MIT**)
- [**Qt 6**](https://www.qt.io/) - optional Core, Gui, Network, and Positioning integration enabled
  with
  `with_qt=True` in Conan or `RCOMMON_WITH_QT=ON` in CMake (**LGPL/GPL/commercial**)
- <img src="./docs/images/logos/catch2.png" alt="" height="16px" width="16px"/>[**catch2**](https://github.com/catchorg/Catch2) - c++ testing
  framework (**BSL-1.0**)

#### CMake targets

- `radar::mms::common` provides the core library without any serialization dependency.
- `radar::mms::common::json` adds the header-only nlohmann/json adapters in
  `<radar/mms/common/json/...>`. It exists only when the package is built with
  `with_nlohmann=True` (default `False`).
- `radar::mms::common::glaze` adds the header-only glaze adapters in
  `<radar/mms/common/glaze/...>`. It exists only when the package is built with `with_glaze=True`
  (default `False`).

With Qt enabled, both adapter targets also cover Qt Core, Gui, and Positioning types. The two
adapter sets write the same JSON, so a document written with one is read by the other. The glaze
adapters also read physical quantities in any unit the quantity has a factory for, e.g.
`{"ampere_hours": 52}` or `{"celsius": 15}`, and, in JSON and TOML, as a string spelled like the
C++ literal without its underscore, e.g. `"52ah"` or `"400hz"`. That suits hand-written TOML
configuration; they still write the SI unit.

Enable the adapters through the consumer's Conan options, e.g. in its `conanfile.py`:

```python
default_options = {"rcommon/*:with_nlohmann": True, "rcommon/*:with_glaze": True}
```

or on the command line with `-o "rcommon/*:with_nlohmann=True" -o "rcommon/*:with_glaze=True"`.

For an installed native CMake package, request the adapter components explicitly:

```cmake
find_package(RCommon REQUIRED COMPONENTS json glaze)
target_link_libraries(my_target PRIVATE radar::mms::common::json radar::mms::common::glaze)
```

#### Licensing

*RCommon* library is distributed under the MIT license. See [LICENSE](./LICENSE) for details.
