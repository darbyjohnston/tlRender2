option(TL_ENABLE_FFMPEG "Enable FFmpeg I/O support" ON)

add_library(tlIO)

target_sources(tlIO
    PRIVATE
        include/tl/IO/Plugin.h
        src/Plugin.cpp)

target_include_directories(tlIO
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tlIO PUBLIC tlCore PRIVATE Threads::Threads)

target_compile_features(tlIO PUBLIC cxx_std_17)

if(TL_ENABLE_FFMPEG)
    find_package(FFMPEG REQUIRED)
    target_sources(tlIO PRIVATE
        src/FFmpegRead.cpp
        include/tl/IO/FFmpegRead.h)
    target_include_directories(tlIO PRIVATE ${FFMPEG_INCLUDE_DIRS})
    target_link_libraries(tlIO PRIVATE ${FFMPEG_LIBRARIES})
    target_link_directories(tlIO PRIVATE ${FFMPEG_LIBRARY_DIRS})
    target_compile_definitions(tlIO PRIVATE TL_IO_HAS_FFMPEG)
endif()

install(TARGETS tlIO
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


