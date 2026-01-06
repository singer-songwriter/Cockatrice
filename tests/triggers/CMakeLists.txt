set(TEST_QT_MODULES ${COCKATRICE_QT_VERSION_NAME}::Core ${COCKATRICE_QT_VERSION_NAME}::Widgets)

add_executable(
  trigger_parser_test
  ../../cockatrice/src/game/triggers/trigger_parser.cpp
  trigger_parser_test.cpp
)

target_include_directories(trigger_parser_test PRIVATE
  ${CMAKE_SOURCE_DIR}/cockatrice/src/game/triggers
  ${CMAKE_SOURCE_DIR}/cockatrice/src/game/cards
  ${CMAKE_SOURCE_DIR}/cockatrice/src/game/board
  ${CMAKE_SOURCE_DIR}/cockatrice/src/game
  ${CMAKE_SOURCE_DIR}/cockatrice/src
  ${CMAKE_SOURCE_DIR}/common
  ${CMAKE_BINARY_DIR}/common
)

if(NOT GTEST_FOUND)
  add_dependencies(trigger_parser_test gtest)
endif()

target_link_libraries(trigger_parser_test cockatrice_protocol Threads::Threads ${GTEST_BOTH_LIBRARIES} ${TEST_QT_MODULES})

add_test(NAME trigger_parser_test COMMAND trigger_parser_test)
