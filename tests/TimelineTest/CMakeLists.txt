set(HEADERS
    TimelineTest.h)

set(SOURCE
    TimelineTest.cpp)

add_library(tlTimelineTest ${HEADERS} ${SOURCE})
target_link_libraries(tlTimelineTest PUBLIC tlTimeline ftkTestLib)
target_include_directories(tlTimelineTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(tlTimelineTest PRIVATE
    TL_TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/tests/data")
set_target_properties(tlTimelineTest PROPERTIES FOLDER tests)
