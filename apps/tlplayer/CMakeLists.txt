add_executable(tlplayer main.cpp)

target_link_libraries(tlplayer PRIVATE tlPlayerApp)

install(TARGETS tlplayer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

