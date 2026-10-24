find_package(Catch2 REQUIRED)

file(GLOB WCOMMON_TEST_SOURCES CONFIGURE_DEPENDS "${WCOMMON_SOURCE_ROOT}/tests/*.cc")
file(GLOB WCOMMON_JSON_TEST_SOURCES CONFIGURE_DEPENDS "${WCOMMON_SOURCE_ROOT}/tests/*_json.cc")
file(GLOB WCOMMON_GLAZE_TEST_SOURCES CONFIGURE_DEPENDS "${WCOMMON_SOURCE_ROOT}/tests/*_glaze.cc")
set(WCOMMON_QT_TEST_SOURCES
  "${WCOMMON_SOURCE_ROOT}/tests/test_geodesy_qt.cc"
  "${WCOMMON_SOURCE_ROOT}/tests/test_geometry_qt.cc"
  "${WCOMMON_SOURCE_ROOT}/tests/test_identifiers_qt.cc"
  "${WCOMMON_SOURCE_ROOT}/tests/test_linear_algebra_qt.cc"
  "${WCOMMON_SOURCE_ROOT}/tests/test_memory_qt.cc"
  "${WCOMMON_SOURCE_ROOT}/tests/test_measurements_qt.cc"
  "${WCOMMON_SOURCE_ROOT}/tests/test_network_qt.cc"
  "${WCOMMON_SOURCE_ROOT}/tests/test_quantities_qt.cc"
)
set(WCOMMON_JSON_QT_TEST_SOURCES "${WCOMMON_SOURCE_ROOT}/tests/test_json_qt.cc")
set(WCOMMON_GLAZE_QT_TEST_SOURCES "${WCOMMON_SOURCE_ROOT}/tests/test_glaze_qt.cc")
list(REMOVE_ITEM WCOMMON_TEST_SOURCES
  "${WCOMMON_SOURCE_ROOT}/tests/test_main.cc"
  ${WCOMMON_JSON_TEST_SOURCES}
  ${WCOMMON_GLAZE_TEST_SOURCES}
  ${WCOMMON_QT_TEST_SOURCES}
  ${WCOMMON_JSON_QT_TEST_SOURCES}
  ${WCOMMON_GLAZE_QT_TEST_SOURCES}
)

add_executable(${PROJECT_NAME}_tests
  "${WCOMMON_SOURCE_ROOT}/tests/test_main.cc"
  ${WCOMMON_TEST_SOURCES}
)

target_link_libraries(${PROJECT_NAME}_tests
  PRIVATE
  ${WCOMMON_TARGET_NAMESPACE}
  Catch2::Catch2
)

if(WCOMMON_WITH_NLOHMANN)
  target_sources(${PROJECT_NAME}_tests PRIVATE ${WCOMMON_JSON_TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${WCOMMON_TARGET_NAMESPACE}::json)
endif()

if(WCOMMON_WITH_GLAZE)
  target_sources(${PROJECT_NAME}_tests PRIVATE ${WCOMMON_GLAZE_TEST_SOURCES})
  target_link_libraries(${PROJECT_NAME}_tests PRIVATE ${WCOMMON_TARGET_NAMESPACE}::glaze)
endif()

if(WCOMMON_WITH_QT)
  target_sources(${PROJECT_NAME}_tests PRIVATE ${WCOMMON_QT_TEST_SOURCES})
  if(WCOMMON_WITH_NLOHMANN)
    target_sources(${PROJECT_NAME}_tests PRIVATE ${WCOMMON_JSON_QT_TEST_SOURCES})
  endif()
  if(WCOMMON_WITH_GLAZE)
    target_sources(${PROJECT_NAME}_tests PRIVATE ${WCOMMON_GLAZE_QT_TEST_SOURCES})
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
