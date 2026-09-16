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
- [**nlohmann/json**](https://github.com/nlohmann/json) - JSON adapters exposed by the opt-in
  `radar::mms::common::json` target (**MIT**)
- [**Qt 6**](https://www.qt.io/) - optional Core, Gui, Network, and Positioning integration enabled
  with
  `with_qt=True` in Conan or `RCOMMON_WITH_QT=ON` in CMake (**LGPL/GPL/commercial**)
- <img src="./docs/images/logos/catch2.png" alt="" height="16px" width="16px"/>[**catch2**](https://github.com/catchorg/Catch2) - c++ testing
  framework (**BSL-1.0**)

#### CMake targets

- `radar::mms::common` provides the core library without a transitive nlohmann/json dependency.
- `radar::mms::common::json` adds the header-only JSON adapters and nlohmann/json dependency.
  With Qt enabled it also provides opt-in adapters for Qt Core, Gui, and Positioning types.

For an installed native CMake package, request the JSON dependency explicitly:

```cmake
find_package(RCommon REQUIRED COMPONENTS json)
target_link_libraries(my_target PRIVATE radar::mms::common::json)
```

#### Licensing

*RCommon* library is distributed under the MIT license. See [LICENSE](./LICENSE) for details.
