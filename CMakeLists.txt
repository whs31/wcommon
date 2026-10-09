cmake_minimum_required(VERSION 3.20)

message(DEBUG "Using ${CMAKE_TOOLCHAIN_FILE}")
message(DEBUG "Tests: ${BUILD_TESTING}")

project(rcommon
  VERSION 4.0.0
  DESCRIPTION "Radar common utility library"
  HOMEPAGE_URL "http://gitlab.uav.radar-mms.com/developers/v2/support/rcommon"
  LANGUAGES CXX
)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(RCOMMON_WITH_QT "Enable Qt integration" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
if(RCOMMON_WITH_QT)
  find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Positioning)
endif()

add_subdirectory(config)

add_library(${PROJECT_NAME})
add_library("radar::mms::common" ALIAS ${PROJECT_NAME})
add_library(${PROJECT_NAME}_json INTERFACE)
add_library("radar::mms::common::json" ALIAS ${PROJECT_NAME}_json)

set_target_properties(${PROJECT_NAME}_json PROPERTIES EXPORT_NAME json)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 23
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  DEFINE_SYMBOL RADAR_COMMON_LIBRARY
  EXPORT_NAME ${PROJECT_NAME}
  LINKER_LANGUAGE CXX
  POSITION_INDEPENDENT_CODE ON
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
  EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/radar/mms/common/detail/export.h"
  EXPORT_MACRO_NAME RADAR_COMMON_API
  STATIC_DEFINE RADAR_COMMON_STATIC_LIBRARY
)

file(GLOB_RECURSE RCOMMON_PUBLIC_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/radar/mms/common/*.h")
file(GLOB RCOMMON_PRIVATE_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/private/*.h"
)

target_sources(${PROJECT_NAME} PRIVATE ${RCOMMON_PUBLIC_HEADERS} ${RCOMMON_PRIVATE_SOURCES})

# Deterministic random streams must round every operation exactly as IEEE 754 prescribes, so
# multiply-adds may not be contracted into FMA instructions (Clang contracts by default, and so
# does GCC in GNU mode or on targets with FMA).
set_source_files_properties(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/random.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/noise.cc"
  PROPERTIES COMPILE_OPTIONS
    "$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-ffp-contract=off>;$<$<CXX_COMPILER_ID:MSVC>:/fp:precise>"
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  radar::mms::common::config

  fmt::fmt
  spdlog::spdlog
)

target_link_libraries(${PROJECT_NAME}_json INTERFACE
  radar::mms::common
  nlohmann_json::nlohmann_json
)

if(RCOMMON_WITH_QT)
  target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Gui Qt6::Network)
  target_link_libraries(${PROJECT_NAME}_json INTERFACE Qt6::Positioning)
  set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON)

  qt_extract_metatypes(${PROJECT_NAME} OUTPUT_FILES RCOMMON_METATYPES_FILE)
  install(
    FILES ${RCOMMON_METATYPES_FILE}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/metatypes
  )
endif()

if(WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

if (NOT BUILD_SHARED_LIBS)
  target_compile_definitions(${PROJECT_NAME} PUBLIC RCOMMON_STATIC_LIBRARY)
endif ()

if (BUILD_TESTING)
  add_subdirectory(tests)
endif ()

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_json
  EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/radar/mms/common"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/radar/mms"
  FILES_MATCHING PATTERN "*.h"
)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/radar/mms/common/detail/export.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/radar/mms/common/detail"
)
install(
  FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RCommonQtDependencies.cmake"
  DESTINATION "cmake_modules"
)
install(EXPORT ${PROJECT_NAME}Targets
  FILE RCommonTargets.cmake
  NAMESPACE radar::mms::common::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RCommon"
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/RCommonConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/RCommonConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RCommon"
)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/RCommonConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
)
install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/RCommonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/RCommonConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/RCommon"
)
