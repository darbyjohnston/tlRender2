add_executable(tl-test tl-test.cpp)
target_link_libraries(tl-test PRIVATE tlCoreTest)
if(TL_ENABLE_FFMPEG)
    target_link_libraries(tl-test PRIVATE tlIOTest)
    target_compile_definitions(tl-test PRIVATE TL_TESTS_HAS_FFMPEG)
endif()
set_target_properties(tl-test PROPERTIES FOLDER tests)

add_test(NAME tl-test COMMAND tl-test)
