find_package(Catch2 REQUIRED)

file(GLOB RCOMMON_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
file(GLOB RCOMMON_JSON_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*_json.cc")
file(GLOB RCOMMON_GLAZE_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*_glaze.cc")
set(RCOMMON_QT_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/test_geodesy_qt.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_geometry_qt.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_identifiers_qt.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_linear_algebra_qt.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_memory_qt.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_measurements_qt.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_network_qt.cc"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_quantities_qt.cc"
)
set(RCOMMON_JSON_QT_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test_json_qt.cc")
set(RCOMMON_GLAZE_QT_TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test_glaze_qt.cc")
list(REMOVE_ITEM RCOMMON_TEST_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/test_main.cc"
  ${RCOMMON_JSON_TEST_SOURCES}
  ${RCOMMON_GLAZE_TEST_SOURCES}
  ${RCOMMON_QT_TEST_SOURCES}
  ${RCOMMON_JSON_QT_TEST_SOURCES}
  ${RCOMMON_GLAZE_QT_TEST_SOURCES}
)

add_executable(${PROJECT_NAME}_tests
  test_main.cc
  ${RCOMMON_TEST_SOURCES}
)

target_link_libraries(${PROJECT_NAME}_tests
  PRIVATE
  radar::mms::common
  Catch2::Catch2
)

if(RCOMMON_WITH_NLOHMANN)
  target_sources(${PROJECT_NAME}_tests PRIVATE ${RCOMMON_JSON_TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}_tests PRIVATE radar::mms::common::json)
endif()

if(RCOMMON_WITH_GLAZE)
  target_sources(${PROJECT_NAME}_tests PRIVATE ${RCOMMON_GLAZE_TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}_tests PRIVATE radar::mms::common::glaze)
endif()

if(RCOMMON_WITH_QT)
  target_sources(${PROJECT_NAME}_tests PRIVATE ${RCOMMON_QT_TEST_SOURCES})
  if(RCOMMON_WITH_NLOHMANN)
    target_sources(${PROJECT_NAME}_tests PRIVATE ${RCOMMON_JSON_QT_TEST_SOURCES})
  endif()
  if(RCOMMON_WITH_GLAZE)
    target_sources(${PROJECT_NAME}_tests PRIVATE ${RCOMMON_GLAZE_QT_TEST_SOURCES})
  endif()
  set_target_properties(${PROJECT_NAME}_tests PROPERTIES AUTOMOC ON)
endif()

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(TARGET ${PROJECT_NAME}_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}_tests>
      $<TARGET_FILE_DIR:${PROJECT_NAME}_tests>
    COMMAND_EXPAND_LISTS
  )
endif ()

add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
