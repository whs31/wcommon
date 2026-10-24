## WCommon

<h3 align="center">
    <img src="./docs/images/icon.png" alt=""/><br/>
    <img src="./docs/images/transparent.png" height="30" width="0px"/>
    Common C++23 utility library
    <img src="./docs/images/transparent.png" height="30" width="0px"/><br/>
    <img src="./docs/images/transparent.png" height="30" width="0px"/><br/>
    <img src="./docs/images/badges/cxx.svg" alt=""/><img src="./docs/images/badges/cmake.svg" alt=""/>
</h3>

*WCommon* is a general-purpose C++23 utility library: physical quantities, geometry, geodesy,
identifiers, networking, logging, deterministic randomness and more, with optional Qt,
nlohmann/json and glaze integration. It is created and maintained by
**whs31**.

#### Dependencies

- <img src="./docs/images/logos/fmt.png" alt="" height="16px" width="16px"/>[**fmt**](https://github.com/fmtlib/fmt) - modern formatting library (**MIT**)
- <img src="./docs/images/logos/spdlog.jpg" alt="" height="16px" width="16px"/>[**spdlog**](https://github.com/gabime/spdlog) - fast C++ logging library (**MIT**)
- [**nlohmann/json**](https://github.com/nlohmann/json) - optional JSON adapters exposed by the
  `whs31::common::json` target, enabled with `with_nlohmann=True` in Conan or
  `WCOMMON_WITH_NLOHMANN=ON` in CMake (**MIT**)
- [**glaze**](https://github.com/stephenberry/glaze) - optional serialization adapters exposed by
  the `whs31::common::glaze` target, enabled with `with_glaze=True` in Conan or
  `WCOMMON_WITH_GLAZE=ON` in CMake (**MIT**)
- [**Qt 6**](https://www.qt.io/) - optional Core, Gui, Network, and Positioning integration enabled
  with
  `with_qt=True` in Conan or `WCOMMON_WITH_QT=ON` in CMake (**LGPL/GPL/commercial**)
- <img src="./docs/images/logos/catch2.png" alt="" height="16px" width="16px"/>[**catch2**](https://github.com/catchorg/Catch2) - c++ testing
  framework (**BSL-1.0**)

#### CMake targets

- `whs31::common` provides the core library without any serialization dependency.
- `whs31::common::json` adds the header-only nlohmann/json adapters in
  `<whs31/common/json/...>`. It exists only when the package is built with
  `with_nlohmann=True` (default `False`).
- `whs31::common::glaze` adds the header-only glaze adapters in
  `<whs31/common/glaze/...>`. It exists only when the package is built with `with_glaze=True`
  (default `False`).

With Qt enabled, both adapter targets also cover Qt Core, Gui, and Positioning types. The two
adapter sets write the same JSON, so a document written with one is read by the other. The glaze
adapters also read physical quantities in any unit the quantity has a factory for, e.g.
`{"ampere_hours": 52}` or `{"celsius": 15}`, and, in JSON and TOML, as a string spelled like the
C++ literal without its underscore, e.g. `"52ah"` or `"400hz"`. That suits hand-written TOML
configuration; they still write the SI unit.

Enable the adapters through the consumer's Conan options, e.g. in its `conanfile.py`:

```python
default_options = {"wcommon/*:with_nlohmann": True, "wcommon/*:with_glaze": True}
```

or on the command line with `-o "wcommon/*:with_nlohmann=True" -o "wcommon/*:with_glaze=True"`.

For an installed native CMake package, request the adapter components explicitly:

```cmake
find_package(WCommon REQUIRED COMPONENTS json glaze)
target_link_libraries(my_target PRIVATE whs31::common::json whs31::common::glaze)
```

#### Namespace

By default the library lives in the `whs31` namespace (with an inline `common` namespace, so
`whs31::Angle` and `whs31::common::Angle` name the same type), its headers are included as
`<whs31/common/...>`, its CMake targets are `whs31::common[::json|::glaze]` and its macros are
prefixed with `WHS31_` (e.g. `WHS31_LOG`, `WHS31_COMMON_API`).

A single option relocates all of it, so that a project can ship the library under its own name, or
link two differently configured copies into one program without conflicts. Set the Conan option
`namespace` (or the CMake cache variable `WCOMMON_NAMESPACE`) to a C++ namespace:

```python
default_options = {"wcommon/*:namespace": "acme::util"}
```

| `namespace`  | C++ namespace | Include path                | CMake target        | Macro prefix |
|--------------|---------------|-----------------------------|---------------------|--------------|
| `whs31`      | `whs31`       | `<whs31/common/angle.h>`    | `whs31::common`     | `WHS31_`     |
| `acme::util` | `acme::util`  | `<acme/util/common/angle.h>`| `acme::util::common`| `ACME_UTIL_` |

The sources are written in the default namespace; any other namespace is built from a respelled
copy that CMake generates in the build tree. An installed package records its namespace in
`WCommon_NAMESPACE` after `find_package(WCommon)`.

#### Licensing

*WCommon* library is distributed under the MIT license. See [LICENSE](./LICENSE) for details.
