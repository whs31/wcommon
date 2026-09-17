include(CMakeFindDependencyMacro)

find_dependency(Qt6 REQUIRED COMPONENTS Core Gui Network)
target_link_libraries(radar::mms::common INTERFACE Qt6::Core Qt6::Gui Qt6::Network)

find_dependency(Qt6 REQUIRED COMPONENTS Positioning)
target_link_libraries(radar::mms::common::json INTERFACE Qt6::Positioning)

get_filename_component(RCommon_PACKAGE_DIR "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
set(RCommon_METATYPES_FILE
  "${RCommon_PACKAGE_DIR}/lib/metatypes/qt6rcommon_metatypes.json"
)
if(NOT EXISTS "${RCommon_METATYPES_FILE}")
  message(FATAL_ERROR "RCommon metatypes file does not exist: ${RCommon_METATYPES_FILE}")
endif()
target_sources(radar::mms::common INTERFACE
  "$<$<BOOL:$<TARGET_PROPERTY:QT_CONSUMES_METATYPES>>:${RCommon_METATYPES_FILE}>"
)
