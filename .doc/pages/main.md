# WCommon Documentation {#mainpage}

**WCommon** is a general-purpose C++23 utility library by whs31. Its
public API lives in the `whs31::common` inline namespace.

This reference uses the default namespace. A package built with the Conan option `namespace`
(CMake `WCOMMON_NAMESPACE`), e.g. `acme::util`, spells every name accordingly: `acme::util::Angle`,
`<acme/util/common/angle.h>`, the `acme::util::common` target and the `ACME_UTIL_` macro prefix.

## CMake targets

- `whs31::common` provides the base library.
- `whs31::common::json` adds the nlohmann/json adapters under
  `whs31/common/json/`. It is built only with the Conan option `with_nlohmann=True`.
- `whs31::common::glaze` adds the glaze adapters under `whs31/common/glaze/`. It is built
  only with the Conan option `with_glaze=True`. See @ref core_glaze.

Both adapter sets write the same JSON. `whs31/common/config.h` defines
`WHS31_COMMON_WITH_NLOHMANN` and `WHS31_COMMON_WITH_GLAZE` when the package was built with them.

Qt integration is controlled by the Conan `with_qt` option. Qt-specific API is
available only when the package is built with `with_qt=True`; each conditional
entity is marked accordingly in the reference documentation.

## Module reference

| Module | Contents |
|--------|----------|
| @ref core_measurements | Physical quantities, units, literals, points, and vectors |
| @ref core_identifiers | UUIDs, semantic versions, and version ranges |
| @ref core_networking | IPv4/IPv6 and socket addresses |
| @ref core_logging | spdlog setup and optional Qt log forwarding |
| @ref core_math | Numeric helpers, range checks, lookup curves, 3×3 matrices and quaternions |
| @ref core_geodesy | WGS84 positions, local tangent planes and Web Mercator tiles |
| @ref core_random | Deterministic random streams and noise processes |
| @ref core_memory | Ownership helpers and pointer utilities |
| @ref core_functional | Lazy values, overload helpers, and scope guards |
| @ref core_threading | Mutex and read/write lock helpers |
| @ref core_filesystem | Lock files, platform paths and per-user application directories |

See @ref quantities for the physical quantity types and their units, @ref rotations for the
vector, matrix and quaternion conventions, and @ref random_streams for reproducible randomness.

Include `whs31/common/all.h` for the complete base API. The serialization adapters remain
opt-in and are provided under `whs31/common/json/` and `whs31/common/glaze/`.
