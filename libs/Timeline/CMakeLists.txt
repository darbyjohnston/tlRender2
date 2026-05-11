add_library(tlTimeline)

target_sources(tlTimeline
    PRIVATE
        include/tl/Timeline/Init.h
        include/tl/Timeline/Player.h
        include/tl/Timeline/Timeline.h
        include/tl/Timeline/VideoGraph.h
        src/Init.cpp
        src/Player.cpp
        src/Timeline.cpp
        src/VideoGraph.cpp)

target_compile_definitions(tlTimeline PRIVATE
    MINIZ_NO_ZLIB_COMPATIBLE_NAMES
    MINIZ_NO_ARCHIVE_WRITING_APIS)
target_sources(tlTimeline
    PRIVATE
    ${PROJECT_SOURCE_DIR}/third_party/miniz/miniz.c)

target_include_directories(tlTimeline
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party/otio/src>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party/miniz>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tlTimeline PUBLIC tlIO PRIVATE OTIO::opentimelineio)

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


