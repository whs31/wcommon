import re

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd


class WCommonRecipe(ConanFile):
  name = "wcommon"
  version = "6.0.0"
  description = "Common C++23 utility library"
  author = "whs31 <whs31@github.io>"
  settings = "os", "arch", "compiler", "build_type"
  license = "MIT"
  url = "http://gitlab.uav.radar-mms.com/developers/v2/support/wcommon"
  exports = "CMakeLists.txt"
  exports_sources = "CMakeLists.txt", "cmake/*", "config/*", "src/*", "include/*", "tests/*"
  options = {
    "shared": [True, False],
    "fPIC": [True, False],
    "test": [True, False],
    "with_qt": [True, False],
    "with_nlohmann": [True, False],
    "with_glaze": [True, False],
    "namespace": ["ANY"],
  }
  default_options = {
    "shared": True,
    "fPIC": True,
    "test": False,
    "with_qt": False,
    "with_nlohmann": False,
    "with_glaze": False,
    "namespace": "whs31",
  }

  user = "radar"
  channel = "dev"
  python_requires = "conan_helpers/1.0@radar/dev"
  python_requires_extend = "conan_helpers.Base"
  cmake_package_prune = ("lib/cmake", "lib/pkgconfig", "res", "share")

  def config_options(self):
    if self.settings.os == "Windows":
      del self.options.fPIC

  def configure(self):
    if self.options.shared:
      self.options.rm_safe("fPIC")

  def validate(self):
    check_min_cppstd(self, "23")
    if not re.fullmatch(r"[A-Za-z_]\w*(::[A-Za-z_]\w*)*", str(self.options.namespace), re.ASCII):
      raise ConanInvalidConfiguration(
        f"namespace '{self.options.namespace}' must be identifiers joined by '::', "
        "e.g. 'whs31' or 'acme::util'"
      )

  @property
  def _target_namespace(self):
    return f"{self.options.namespace}::common"

  @property
  def _macro_prefix(self):
    return str(self.options.namespace).replace("::", "_").upper()

  def requirements(self):
    self.req(
      "spdlog",
      options={"shared": True},
      transitive_headers=True,
      transitive_libs=True,
    )
    self.req("fmt", transitive_headers=True, transitive_libs=True)
    if self.options.with_nlohmann:
      self.req("nlohmann_json", transitive_headers=True)
    if self.options.with_glaze:
      self.req("glaze", transitive_headers=True)
    if self.options.test:
      self.req("catch2", test=True)

  def configure_cmake_toolchain(self, toolchain):
    toolchain.shared = self.options.shared
    toolchain.cache_variables["BUILD_TESTING"] = bool(self.options.test)
    toolchain.cache_variables["WCOMMON_WITH_QT"] = bool(self.options.with_qt)
    toolchain.cache_variables["WCOMMON_WITH_NLOHMANN"] = bool(self.options.with_nlohmann)
    toolchain.cache_variables["WCOMMON_WITH_GLAZE"] = bool(self.options.with_glaze)
    toolchain.cache_variables["WCOMMON_NAMESPACE"] = str(self.options.namespace)

  def package_info(self):
    self.cpp_info.set_property("cmake_file_name", "WCommon")
    if self.options.with_qt:
      self.cpp_info.set_property(
        "cmake_build_modules",
        ["cmake_modules/WCommonQtDependencies.cmake"],
      )
    common = self.cpp_info.components["common"]
    common.set_property("cmake_target_name", self._target_namespace)
    common.libs = ["wcommon"]
    if not self.options.shared:
      common.defines = [f"{self._macro_prefix}_COMMON_STATIC_LIBRARY"]
    if self.settings.os == "Windows":
      common.system_libs = ["ws2_32"]
    common.requires = [
      "spdlog::spdlog",
      "fmt::fmt",
    ]

    if self.options.with_nlohmann:
      json = self.cpp_info.components["json"]
      json.set_property("cmake_target_name", f"{self._target_namespace}::json")
      json.requires = [
        "common",
        "nlohmann_json::nlohmann_json",
      ]

    if self.options.with_glaze:
      glaze = self.cpp_info.components["glaze"]
      glaze.set_property("cmake_target_name", f"{self._target_namespace}::glaze")
      glaze.requires = [
        "common",
        "glaze::glaze",
      ]
