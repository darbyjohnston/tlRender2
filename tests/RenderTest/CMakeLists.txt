set(HEADERS
    RenderTest.h
    SessionTest.h)

set(SOURCE
    RenderTest.cpp
    SessionTest.cpp)

add_library(tlRenderTest ${HEADERS} ${SOURCE})
target_link_libraries(tlRenderTest PUBLIC tlRender ftkTestLib)
target_include_directories(tlRenderTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(tlRenderTest PRIVATE
    TL_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data")
set_target_properties(tlRenderTest PROPERTIES FOLDER tests)
