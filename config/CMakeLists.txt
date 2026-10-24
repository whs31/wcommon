project(wcommon_config LANGUAGES NONE)

add_library(${PROJECT_NAME} INTERFACE)
add_library("${WCOMMON_TARGET_NAMESPACE}::config" ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME config)

# config.h.in tests the feature macros by their prefixed name, so they are set under that name.
foreach(feature IN ITEMS QT NLOHMANN GLAZE)
  set(${WCOMMON_MACRO_PREFIX}_COMMON_WITH_${feature} ${WCOMMON_WITH_${feature}})
endforeach()

configure_file(
  "${WCOMMON_SOURCE_ROOT}/config/config.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${WCOMMON_INCLUDE_PREFIX}/config.h"
  @ONLY
)

target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${WCOMMON_INCLUDE_PREFIX}/config.h"
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${WCOMMON_INCLUDE_PREFIX}"
)

install(TARGETS ${PROJECT_NAME}
  EXPORT wcommonTargets
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
