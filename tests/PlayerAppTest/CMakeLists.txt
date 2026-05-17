set(HEADERS
    FilesModelTest.h)

set(SOURCE
    FilesModelTest.cpp)

add_library(tlPlayerAppTest ${HEADERS} ${SOURCE})
target_link_libraries(tlPlayerAppTest PUBLIC tlPlayerApp ftkTestLib)
target_include_directories(tlPlayerAppTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(tlPlayerAppTest PRIVATE
    TL_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data")
set_target_properties(tlPlayerAppTest PROPERTIES FOLDER tests)
