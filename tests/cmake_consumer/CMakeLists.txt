cmake_minimum_required(VERSION 3.20)

project(rcommon_consumer LANGUAGES CXX)

set(RCOMMON_CONSUMER_COMPONENT "core" CACHE STRING "RCommon component to test: core, json or glaze")
set_property(CACHE RCOMMON_CONSUMER_COMPONENT PROPERTY STRINGS core json glaze)

if(RCOMMON_CONSUMER_COMPONENT STREQUAL "json")
  find_package(RCommon REQUIRED COMPONENTS json)
  add_executable(rcommon_consumer json.cc)
  target_link_libraries(rcommon_consumer PRIVATE radar::mms::common::json)
elseif(RCOMMON_CONSUMER_COMPONENT STREQUAL "glaze")
  find_package(RCommon REQUIRED COMPONENTS glaze)
  add_executable(rcommon_consumer glaze.cc)
  target_link_libraries(rcommon_consumer PRIVATE radar::mms::common::glaze)
elseif(RCOMMON_CONSUMER_COMPONENT STREQUAL "core")
  find_package(RCommon REQUIRED)
  add_executable(rcommon_consumer core.cc)
  target_link_libraries(rcommon_consumer PRIVATE radar::mms::common)
else()
  message(FATAL_ERROR "Unknown RCOMMON_CONSUMER_COMPONENT: ${RCOMMON_CONSUMER_COMPONENT}")
endif()

target_compile_features(rcommon_consumer PRIVATE cxx_std_23)
