cmake_minimum_required(VERSION 3.20)

message(DEBUG "Using ${CMAKE_TOOLCHAIN_FILE}")
message(DEBUG "Tests: ${BUILD_TESTING}")

project(wcommon
  VERSION 6.0.0
  DESCRIPTION "Common C++23 utility library"
  HOMEPAGE_URL "http://gitlab.uav.radar-mms.com/developers/v2/support/wcommon"
  LANGUAGES CXX
)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/WCommonNamespace.cmake")

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(WCOMMON_WITH_QT "Enable Qt integration" OFF)
option(WCOMMON_WITH_NLOHMANN "Build the nlohmann/json adapters (<namespace>::common::json)" OFF)
option(WCOMMON_WITH_GLAZE "Build the glaze adapters (<namespace>::common::glaze)" OFF)
set(WCOMMON_NAMESPACE "${WCOMMON_SOURCE_NAMESPACE}" CACHE STRING
  "C++ namespace of the library, e.g. 'whs31' or 'acme::util'; it also selects the include path \
(<acme/util/common/...>), the CMake target namespace (acme::util::common) and the macro prefix (ACME_UTIL_)"
)

wcommon_namespace_properties("${WCOMMON_NAMESPACE}" WCOMMON_NAMESPACE_PATH WCOMMON_MACRO_PREFIX)
set(WCOMMON_INCLUDE_PREFIX "${WCOMMON_NAMESPACE_PATH}/common")
set(WCOMMON_TARGET_NAMESPACE "${WCOMMON_NAMESPACE}::common")

# Sources of the library, its configuration header and its tests, spelled in WCOMMON_NAMESPACE.
if(WCOMMON_NAMESPACE STREQUAL WCOMMON_SOURCE_NAMESPACE)
  set(WCOMMON_SOURCE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
else()
  set(WCOMMON_SOURCE_ROOT "${CMAKE_CURRENT_BINARY_DIR}/wcommon-sources")
  file(GLOB_RECURSE WCOMMON_REWRITTEN_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
  )
  file(GLOB WCOMMON_REWRITTEN_SUPPORT_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/config/*.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc"
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h"
  )
  wcommon_rewrite_namespace(
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    DESTINATION_DIR "${WCOMMON_SOURCE_ROOT}"
    NAMESPACE "${WCOMMON_NAMESPACE}"
    FILES ${WCOMMON_REWRITTEN_SOURCES} ${WCOMMON_REWRITTEN_SUPPORT_FILES}
  )
  message(STATUS "WCommon: building in namespace ${WCOMMON_NAMESPACE} from ${WCOMMON_SOURCE_ROOT}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
if(WCOMMON_WITH_NLOHMANN)
  find_package(nlohmann_json REQUIRED)
endif()
if(WCOMMON_WITH_GLAZE)
  find_package(glaze REQUIRED)
endif()
if(WCOMMON_WITH_QT)
  find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Positioning)
endif()

add_subdirectory(config)

add_library(${PROJECT_NAME})
add_library("${WCOMMON_TARGET_NAMESPACE}" ALIAS ${PROJECT_NAME})
set(WCOMMON_INSTALL_TARGETS ${PROJECT_NAME})

if(WCOMMON_WITH_NLOHMANN)
  add_library(${PROJECT_NAME}_json INTERFACE)
  add_library("${WCOMMON_TARGET_NAMESPACE}::json" ALIAS ${PROJECT_NAME}_json)
  set_target_properties(${PROJECT_NAME}_json PROPERTIES EXPORT_NAME json)
  list(APPEND WCOMMON_INSTALL_TARGETS ${PROJECT_NAME}_json)
endif()

if(WCOMMON_WITH_GLAZE)
  add_library(${PROJECT_NAME}_glaze INTERFACE)
  add_library("${WCOMMON_TARGET_NAMESPACE}::glaze" ALIAS ${PROJECT_NAME}_glaze)
  set_target_properties(${PROJECT_NAME}_glaze PROPERTIES EXPORT_NAME glaze)
  list(APPEND WCOMMON_INSTALL_TARGETS ${PROJECT_NAME}_glaze)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  DEFINE_SYMBOL ${WCOMMON_MACRO_PREFIX}_COMMON_LIBRARY
  EXPORT_NAME ${PROJECT_NAME}
  LINKER_LANGUAGE CXX
  POSITION_INDEPENDENT_CODE ON
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
  BASE_NAME ${WCOMMON_MACRO_PREFIX}_COMMON
  EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/${WCOMMON_INCLUDE_PREFIX}/detail/export.h"
  EXPORT_MACRO_NAME ${WCOMMON_MACRO_PREFIX}_COMMON_API
  STATIC_DEFINE ${WCOMMON_MACRO_PREFIX}_COMMON_STATIC_LIBRARY
)

file(GLOB_RECURSE WCOMMON_PUBLIC_HEADERS CONFIGURE_DEPENDS "${WCOMMON_SOURCE_ROOT}/include/${WCOMMON_INCLUDE_PREFIX}/*.h")
# The serialization adapters belong to their optional interface targets, not to the core library.
list(FILTER WCOMMON_PUBLIC_HEADERS EXCLUDE REGEX "/include/${WCOMMON_INCLUDE_PREFIX}/(json|glaze)/")
file(GLOB WCOMMON_PRIVATE_SOURCES CONFIGURE_DEPENDS
  "${WCOMMON_SOURCE_ROOT}/src/*.cc"
  "${WCOMMON_SOURCE_ROOT}/src/private/*.h"
)

target_sources(${PROJECT_NAME} PRIVATE ${WCOMMON_PUBLIC_HEADERS} ${WCOMMON_PRIVATE_SOURCES})

# Deterministic random streams must round every operation exactly as IEEE 754 prescribes, so
# multiply-adds may not be contracted into FMA instructions (Clang contracts by default, and so
# does GCC in GNU mode or on targets with FMA).
set_source_files_properties(
  "${WCOMMON_SOURCE_ROOT}/src/random.cc"
  "${WCOMMON_SOURCE_ROOT}/src/noise.cc"
  PROPERTIES COMPILE_OPTIONS
    "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-ffp-contract=off>;$<$<CXX_COMPILER_ID:MSVC>:/fp:precise>"
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${WCOMMON_SOURCE_ROOT}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
  ${WCOMMON_SOURCE_ROOT}/src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  ${WCOMMON_TARGET_NAMESPACE}::config

  fmt::fmt
  spdlog::spdlog
)

if(WCOMMON_WITH_NLOHMANN)
  target_link_libraries(${PROJECT_NAME}_json INTERFACE
    ${WCOMMON_TARGET_NAMESPACE}
    nlohmann_json::nlohmann_json
  )
endif()

if(WCOMMON_WITH_GLAZE)
  target_link_libraries(${PROJECT_NAME}_glaze INTERFACE
    ${WCOMMON_TARGET_NAMESPACE}
    glaze::glaze
  )
endif()

if(WCOMMON_WITH_QT)
  target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Gui Qt6::Network)
  if(WCOMMON_WITH_NLOHMANN)
    target_link_libraries(${PROJECT_NAME}_json INTERFACE Qt6::Positioning)
  endif()
  if(WCOMMON_WITH_GLAZE)
    target_link_libraries(${PROJECT_NAME}_glaze INTERFACE Qt6::Positioning)
  endif()
  set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)

  qt_extract_metatypes(${PROJECT_NAME} OUTPUT_FILES WCOMMON_METATYPES_FILE)
  install(
    FILES ${WCOMMON_METATYPES_FILE}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/metatypes
  )
endif()

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

if (NOT BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME} PUBLIC ${WCOMMON_MACRO_PREFIX}_COMMON_STATIC_LIBRARY)
endif ()

if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()

install(TARGETS ${WCOMMON_INSTALL_TARGETS}
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY "${WCOMMON_SOURCE_ROOT}/include/${WCOMMON_INCLUDE_PREFIX}"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${WCOMMON_NAMESPACE_PATH}"
  FILES_MATCHING PATTERN "*.h"
  PATTERN "json" EXCLUDE
  PATTERN "glaze" EXCLUDE
)
if(WCOMMON_WITH_NLOHMANN)
  install(
    DIRECTORY "${WCOMMON_SOURCE_ROOT}/include/${WCOMMON_INCLUDE_PREFIX}/json"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${WCOMMON_INCLUDE_PREFIX}"
    FILES_MATCHING PATTERN "*.h"
  )
endif()
if(WCOMMON_WITH_GLAZE)
  install(
    DIRECTORY "${WCOMMON_SOURCE_ROOT}/include/${WCOMMON_INCLUDE_PREFIX}/glaze"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${WCOMMON_INCLUDE_PREFIX}"
    FILES_MATCHING PATTERN "*.h"
  )
endif()
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${WCOMMON_INCLUDE_PREFIX}/detail/export.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${WCOMMON_INCLUDE_PREFIX}/detail"
)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/WCommonQtDependencies.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/WCommonQtDependencies.cmake"
  @ONLY
)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/WCommonQtDependencies.cmake"
  DESTINATION "cmake_modules"
)
install(EXPORT ${PROJECT_NAME}Targets
  FILE WCommonTargets.cmake
  NAMESPACE ${WCOMMON_TARGET_NAMESPACE}::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/WCommon"
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/WCommonConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/WCommonConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/WCommon"
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/WCommonConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/WCommonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/WCommonConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/WCommon"
)
