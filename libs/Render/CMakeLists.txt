add_library(tlRender)

target_sources(tlRender
    PRIVATE
        include/tl/Render/Render.h
        include/tl/Render/RenderUtil.h
        include/tl/Render/Session.h
        src/OIIOUtil.cpp
        src/OIIOUtil.h
        src/Render.cpp
        src/RenderUtil.cpp
        src/Session.cpp)

target_include_directories(tlRender
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tlRender PUBLIC tlTimeline)

target_compile_features(tlRender PUBLIC cxx_std_17)

install(TARGETS tlRender
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


