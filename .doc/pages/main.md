# RCommon Documentation {#mainpage}

**RCommon** is the shared C++23 utility library for Radar MMS projects. Its
public API lives in the `radar::mms::common` inline namespace.

## CMake targets

- `radar::mms::common` provides the base library.
- `radar::mms::common::json` adds the opt-in nlohmann JSON adapters.

Qt integration is controlled by the Conan `with_qt` option. Qt-specific API is
available only when the package is built with `with_qt=True`; each conditional
entity is marked accordingly in the reference documentation.

## Module reference

| Module | Contents |
|--------|----------|
| @ref core_measurements | Angles, distances, velocities, points, and vectors |
| @ref core_identifiers | UUIDs, semantic versions, and version ranges |
| @ref core_networking | IPv4/IPv6 and socket addresses |
| @ref core_logging | spdlog setup and optional Qt log forwarding |
| @ref core_math | Numeric helpers and interpolation |
| @ref core_memory | Ownership helpers and pointer utilities |
| @ref core_functional | Lazy values, overload helpers, and scope guards |
| @ref core_threading | Mutex and read/write lock helpers |
| @ref core_filesystem | Lock files and platform paths |

Include `radar/mms/common/all.h` for the complete base API. JSON adapters remain
opt-in and are provided under `radar/mms/common/json/`.
