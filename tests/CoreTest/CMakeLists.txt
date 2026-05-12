set(HEADERS
    AudioTest.h
    TimeTest.h)

set(SOURCE
    AudioTest.cpp
    TimeTest.cpp)

add_library(tlCoreTest ${HEADERS} ${SOURCE})
target_link_libraries(tlCoreTest PUBLIC tlCore ftkTestLib)
target_include_directories(tlCoreTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(tlCoreTest PRIVATE
    TL_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data")
set_target_properties(tlCoreTest PROPERTIES FOLDER tests)
