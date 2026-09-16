cmake_minimum_required(VERSION 3.20)

project(rcommon_consumer LANGUAGES CXX)

option(RCOMMON_CONSUMER_JSON "Test the JSON component" OFF)

if(RCOMMON_CONSUMER_JSON)
  find_package(RCommon REQUIRED COMPONENTS json)
  add_executable(rcommon_consumer json.cc)
  target_link_libraries(rcommon_consumer PRIVATE radar::mms::common::json)
else()
  find_package(RCommon REQUIRED)
  add_executable(rcommon_consumer core.cc)
  target_link_libraries(rcommon_consumer PRIVATE radar::mms::common)
endif()

target_compile_features(rcommon_consumer PRIVATE cxx_std_23)
