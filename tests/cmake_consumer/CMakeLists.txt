cmake_minimum_required(VERSION 3.20)

project(wcommon_consumer LANGUAGES CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/WCommonNamespace.cmake")

set(WCOMMON_CONSUMER_COMPONENT "core" CACHE STRING "WCommon component to test: core, json or glaze")
set_property(CACHE WCOMMON_CONSUMER_COMPONENT PROPERTY STRINGS core json glaze)

if(WCOMMON_CONSUMER_COMPONENT STREQUAL "json")
  find_package(WCommon REQUIRED COMPONENTS json)
  set(target_suffix "::json")
elseif(WCOMMON_CONSUMER_COMPONENT STREQUAL "glaze")
  find_package(WCommon REQUIRED COMPONENTS glaze)
  set(target_suffix "::glaze")
elseif(WCOMMON_CONSUMER_COMPONENT STREQUAL "core")
  find_package(WCommon REQUIRED)
  set(target_suffix "")
else()
  message(FATAL_ERROR "Unknown WCOMMON_CONSUMER_COMPONENT: ${WCOMMON_CONSUMER_COMPONENT}")
endif()

# The sources are spelled in the default namespace; respell them in the one the package was built in.
set(source_dir "${CMAKE_CURRENT_SOURCE_DIR}")
if(NOT WCommon_NAMESPACE STREQUAL WCOMMON_SOURCE_NAMESPACE)
  set(source_dir "${CMAKE_CURRENT_BINARY_DIR}/sources")
  wcommon_rewrite_namespace(
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
    DESTINATION_DIR "${source_dir}"
    NAMESPACE "${WCommon_NAMESPACE}"
    FILES core.cc json.cc glaze.cc
  )
endif()

add_executable(wcommon_consumer "${source_dir}/${WCOMMON_CONSUMER_COMPONENT}.cc")
target_link_libraries(wcommon_consumer PRIVATE ${WCommon_NAMESPACE}::common${target_suffix})
target_compile_features(wcommon_consumer PRIVATE cxx_std_23)
