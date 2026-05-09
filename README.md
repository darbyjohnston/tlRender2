[![CI](https://github.com/darbyjohnston/tlRender2/actions/workflows/ci.yml/badge.svg)](https://github.com/darbyjohnston/tlRender2/actions/workflows/ci.yml)

# tlRender2

A library for rendering timelines.

Homepage: https://github.com/darbyjohnston/tlRender2

## Overview

tlRender2 provides a set of C++ libraries for working with media timelines:

- **tlCore** — core utilities and types
- **tlIO** — media I/O, including optional FFmpeg-based readers
- **tlTimeline** — timeline data structures and playback
- **tlUI** — timeline-related UI widgets

Two example applications are included:

- **tlrender** — example command-line renderer
- **tlplayer** — example player using the UI library

## Building

### Requirements

- A C++17 compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- CMake 3.24 or newer
- Git
- An internet connection for the first build (to fetch dependencies)

Dependencies are managed by [vcpkg](https://vcpkg.io/), which is included as a
git submodule. CMake will initialize and bootstrap it automatically on the
first configure.

### Quick start

```bash
git clone --recursive https://github.com/darbyjohnston/tlRender2.git
cd tlRender
cmake --preset default
cmake --build build
```

If you forget `--recursive`, the first `cmake` invocation will detect this
and initialize the submodule for you.

> **Note:** The first configure will take 10–20 minutes and several hundred
> megabytes of disk space, because vcpkg builds dependencies from source.
> Subsequent configures reuse the cache and are much faster.

### Build options

| Option              | Default | Description                |
| ------------------- | ------- | -------------------------- |
| `TL_ENABLE_FFMPEG`  | `ON`    | Enable FFmpeg I/O support  |

Pass options at configure time:

```bash
cmake --preset default -DTL_ENABLE_FFMPEG=OFF
```

### Installing

```bash
cmake --install build --prefix /your/install/prefix
```

This installs the libraries, headers, executables, and a CMake package
config file so downstream projects can use `find_package(tlRender2)`.

## Using tlRender2 from another project

After installing, consumers can find and link tlRender2 through standard
CMake:

```cmake
find_package(tlRender2 2.0 REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE tlRender2::tlTimeline)
```

Available targets are `tlRender2::tlCore`, `tlRender2::tlIO`,
`tlRender2::tlTimeline`, and `tlRender2::tlUI`. Linking any of them pulls
in its dependencies transitively.

Headers live under the `tl/` prefix, mirroring the library layout:

```cpp
#include <tl/Core/Audio.h>
#include <tl/IO/Plugin.h>
#include <tl/Timeline/Player.h>
#include <tl/UI/TimelineWidget.h>
```

## License

BSD-3-Clause. See [LICENSE.txt](LICENSE.txt).
