# PrismSurface
PrismSurface is a cross-platform windowing library, managing windowing and input OS level and providing cross platform interface and independence.

<span style="color: Crimson;">**PrismSurface is still in development, most of the features have not been implemented yet.**</span>

## Supported platforms
- Windows 10/11

Linux and MacOS are planned in the future.

## Requirements
PrismSurface is independent on third-party libraries except OS sdk.
- CMake `3.16` or higher
- Windows sdk for windows build

## Building
Prism surface uses CMake as its build system.
[`Scripts`](./Scripts) directory contains scripts for automatic build and cleanup, every build script passes its arguments to cmake, so you can alter cmake options freely.

See all cmake options in the table below.

| Options                  | Usage                               | Values                  |
|:-------------------------|:-----------------------------------:|:-----------------------:|
| BUILD_SHARED_LIBS        | Define library type                 | ON/**OFF**              |
| PRISM_LIBRARY_TYPE       | Override CMakes's BUILD_SHARED_LIBS | **AUTO**/STATIC/SHARED  |
| USE_SOURCE_HEADERS       | Use internal headers                | ON/**OFF**              |
| PRISM_FORCE_ERRORS       | Treat library warnings as errors    | **ON**/OFF              |
| PRISM_BUILD_EXAMPLES     | Build example project               | **ON**/OFF              |
| PRISM_BUILD_TESTS        | Build test project                  | ON/**OFF**              |

*Default values are bold*

`BUILD_SHARED_LIBS` specifies library type across whole project.<br>
`PRISM_LIBRARY_TYPE` can override BUILD_SHARED_LIBS specifically for the library. If set to `AUTO`, CMake will use `BUILD_SHARED_LIBS`.<br>
`USE_SOURCE_HEADERS` gives you access to platform specific headers.<br>

See all compiler macros in the table below.

| Macros                   | Usage                                                           |
|:-------------------------|:---------------------------------------------------------------:|
| PRISM_API                | Exports/Imports library symbols                                 |
| PRISM_SOURCE_API         | Exports/Imports library internal symbols for USE_SOURCE_HEADERS |
| PRISM_PLATFORM_WINDOWS   | Specifies Windows platform                                      |
| PRISMSURFACE_DEBUG       | Specifies debug configuration                                   |
| PRISMSURFACE_RELEASE     | Specifies release configuration                                 |

### Output
Executables (including dlls) are output to `bin` directory.
Libraries are output to `lib` directory.

### Supported IDEs and configurations
Supported IDEs are Visual Studio 2022 and 2026, CLion and Makefiles, although you can use whatever IDE cmake supports.
Target configurations are `Debug` and `Release`.

## Using PrismSurface
### Installing
Run ```cmake --install``` in your build directory to get final library and header files.
Required directories are all installed within your install directory.
If `USE_SOURCE_HEADERS` is on it also installs `src` directory which contains internal platform specific header files.

All installed directories are needed and are automatically configured to linked project.

`include` directory contains public interface headers.
`src` directory contains platform specific headers, `USE_SOURCE_HEADERS` option required.
`config` directory contains configuration headers such as library version and dynamic symbol export macros.

### Using with CMake
```
find_package(PrismSurface REQUIRED)
target_link_libraries(App PRIVATE PrismSurface::PrismSurface)
```
