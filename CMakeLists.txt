# PrismSurface project cmake

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(PrismSurface VERSION 0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default cmake option to build shared libraries, can be overwritten by PRISM_LIBRARY_TYPE
option(BUILD_SHARED_LIBS "Build shared libraries, can be overwritten by PRISM_LIBRARY_TYPE" OFF)
option(USE_SOURCE_HEADERS "Install source headers, can be useful when using specific platform stuff" OFF)
option(PRISM_FORCE_ERRORS "Force compiler to treat warnings as errors" ON)
option(PRISM_BUILD_EXAMPLES "Build the ${PROJECT_NAME} example projects" ON)
option(PRISM_BUILD_TESTS "Build the ${PROJECT_NAME} test projects" OFF)

# If set to AUTO (default), library type will be determined by BUILD_SHARED_LIBS
set(PRISM_LIBRARY_TYPE "AUTO" CACHE STRING "Choose the type of ${PROJECT_NAME} library to build (STATIC, SHARED or AUTO)")

set_property(CACHE PRISM_LIBRARY_TYPE PROPERTY STRINGS "STATIC" "SHARED" "AUTO")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (PRISM_LIBRARY_TYPE STREQUAL "AUTO")
    if(BUILD_SHARED_LIBS)
        set(PRISM_LIBRARY_TYPE "SHARED")
    else()
        set(PRISM_LIBRARY_TYPE "STATIC")
    endif()
endif()

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Available build configurations" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")

configure_file("${PROJECT_SOURCE_DIR}/CMake/PrismSurfaceConfig.h.in" "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}/config/PrismSurfaceConfig.h")

if (WIN32)
    add_compile_definitions(PRISM_PLATFORM_WINDOWS)
endif()

add_subdirectory(${PROJECT_NAME})

if (PRISM_BUILD_EXAMPLES)
    add_subdirectory(Examples)
endif()

if (PRISM_BUILD_TESTS)
    add_subdirectory(Tests)
endif()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "PrismSurfaceConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY "SameMajorVersion"
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/CMake/PrismSurfaceConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/PrismSurfaceConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/"
)

install(EXPORT PrismSurfaceTargets
    FILE PrismSurfaceTargets.cmake
    NAMESPACE PrismSurface::
    DESTINATION "lib/cmake/"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/PrismSurfaceConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/PrismSurfaceConfigVersion.cmake"
    DESTINATION "lib/cmake/"
)
