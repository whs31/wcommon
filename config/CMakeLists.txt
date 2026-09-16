project(rcommon_config LANGUAGES NONE)

add_library(${PROJECT_NAME} INTERFACE)
add_library("radar::mms::common::config" ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME config)

configure_file(
  config.h.in
  "${CMAKE_CURRENT_BINARY_DIR}/radar/mms/common/config.h"
  @ONLY
)

target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/radar/mms/common/config.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/radar/mms/common"
)

install(TARGETS ${PROJECT_NAME}
  EXPORT rcommonTargets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
