add_library(tlTimeline)

target_sources(tlTimeline
    PRIVATE
        include/tl/Timeline/Player.h
        src/Player.cpp)

target_include_directories(tlTimeline
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tlTimeline PUBLIC tlIO)

target_compile_features(tlTimeline PUBLIC cxx_std_17)

install(TARGETS tlTimeline
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


