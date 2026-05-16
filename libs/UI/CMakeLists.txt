add_library(tlUI)

target_sources(tlUI
    PRIVATE
        include/tl/UI/DurationEdit.h
        include/tl/UI/DurationLabel.h
        include/tl/UI/TimeEdit.h
        include/tl/UI/TimeLabel.h
        include/tl/UI/TimelineWidget.h
        include/tl/UI/TimeUnitsModel.h
        include/tl/UI/TimeUnitsWidget.h
        include/tl/UI/Viewport.h
        src/DurationEdit.cpp
        src/DurationLabel.cpp
        src/TimeEdit.cpp
        src/TimeLabel.cpp
        src/TimelineWidget.cpp
        src/TimeUnitsModel.cpp
        src/TimeUnitsWidget.cpp
        src/Viewport.cpp)

target_include_directories(tlUI
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(tlUI PUBLIC tlTimeline ftk::ftkUI)

target_compile_features(tlUI PUBLIC cxx_std_17)

install(TARGETS tlUI
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


