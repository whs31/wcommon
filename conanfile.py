from conan import ConanFile
from conan.tools.build import check_min_cppstd


class RCommonRecipe(ConanFile):
  name = "rcommon"
  version = "3.0.2"
  description = "Radar common utility library"
  author = "whs31 <whs31@github.io>"
  settings = "os", "arch", "compiler", "build_type"
  license = "MIT"
  url = "http://gitlab.uav.radar-mms.com/developers/v2/support/rcommon"
  exports = "CMakeLists.txt"
  exports_sources = "CMakeLists.txt", "cmake/*", "config/*", "src/*", "include/*", "tests/*"
  options = {
    "shared": [True, False],
    "fPIC": [True, False],
    "test": [True, False],
    "with_qt": [True, False],
  }
  default_options = {
    "shared": True,
    "fPIC": True,
    "test": False,
    "with_qt": False,
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

  def requirements(self):
    self.req(
      "spdlog",
      options={"shared": True},
      transitive_headers=True,
      transitive_libs=True,
    )
    self.req("fmt", transitive_headers=True, transitive_libs=True)
    self.req("nlohmann_json", transitive_headers=True)
    if self.options.test:
      self.req("catch2", test=True)

  def configure_cmake_toolchain(self, toolchain):
    toolchain.shared = self.options.shared
    toolchain.cache_variables["BUILD_TESTING"] = bool(self.options.test)
    toolchain.cache_variables["RCOMMON_WITH_QT"] = bool(self.options.with_qt)

  def package_info(self):
    self.cpp_info.set_property("cmake_file_name", "RCommon")
    if self.options.with_qt:
      self.cpp_info.set_property(
        "cmake_build_modules",
        ["cmake_modules/RCommonQtDependencies.cmake"],
      )
    common = self.cpp_info.components["common"]
    common.set_property("cmake_target_name", "radar::mms::common")
    common.libs = ["rcommon"]
    if not self.options.shared:
      common.defines = ["RADAR_COMMON_STATIC_LIBRARY"]
    if self.settings.os == "Windows":
      common.system_libs = ["ws2_32"]
    common.requires = [
      "spdlog::spdlog",
      "fmt::fmt",
    ]

    json = self.cpp_info.components["json"]
    json.set_property("cmake_target_name", "radar::mms::common::json")
    json.requires = [
      "common",
      "nlohmann_json::nlohmann_json",
    ]
