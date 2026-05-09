set(HEADERS
    AudioResampleTest.h)

set(SOURCE
    AudioResampleTest.cpp)

add_library(tlIOTest ${HEADERS} ${SOURCE})
target_link_libraries(tlIOTest PUBLIC tlIO ftkTestLib)
target_include_directories(tlIOTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set_target_properties(tlIOTest PROPERTIES FOLDER tests)
