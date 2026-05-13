add_library(tlPlayerApp)

target_sources(tlPlayerApp
    PRIVATE
        include/tl/PlayerApp/App.h
        include/tl/PlayerApp/MainWindow.h
        src/App.cpp
        src/MainWindow.cpp)

target_include_directories(tlPlayerApp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tlPlayerApp PUBLIC tlUI)

target_compile_features(tlPlayerApp PUBLIC cxx_std_17)

install(TARGETS tlPlayerApp
    EXPORT tlRender2Targets
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "._*" EXCLUDE)


