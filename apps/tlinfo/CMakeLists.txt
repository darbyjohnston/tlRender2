add_executable(tlinfo main.cpp)

target_link_libraries(tlinfo PRIVATE tlInfoApp)

install(TARGETS tlinfo
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
