add_executable(tlrender main.cpp)

target_link_libraries(tlrender PRIVATE tlTimeline)

install(TARGETS tlrender
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
