add_executable(tl-test tl-test.cpp)
target_link_libraries(tl-test PRIVATE tlRenderTest tlTimelineTest tlIOTest tlCoreTest)
target_compile_definitions(tl-test PRIVATE
    TL_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data")
set_target_properties(tl-test PROPERTIES FOLDER tests)

add_test(NAME tl-test COMMAND tl-test)
