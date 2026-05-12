add_executable(tlrender main.cpp)

target_link_libraries(tlrender PRIVATE tlRenderApp)

install(TARGETS tlrender
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
