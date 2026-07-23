# PrismSurface library cmake

if(USE_SOURCE_HEADERS)
    set(SOURCE_VISIBILITY "PUBLIC")
else()
    set(SOURCE_VISIBILITY "PRIVATE")
endif()

file (GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library("${PROJECT_NAME}" "${PRISM_LIBRARY_TYPE}" "${SOURCES}")

target_include_directories("${PROJECT_NAME}"
    PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
    ${SOURCE_VISIBILITY}
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>"
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/config>"
        "$<INSTALL_INTERFACE:config>"

        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
)

if(SOURCE_VISIBILITY STREQUAL "PUBLIC")
    target_include_directories("${PROJECT_NAME}" PUBLIC "$<INSTALL_INTERFACE:src>")
endif()

if (WIN32)
    target_link_libraries("${PROJECT_NAME}"
        PRIVATE
            "user32"
            "gdi32"
            "dwmapi"
            "ole32"
    )

    target_compile_definitions("${PROJECT_NAME}"
        PRIVATE
            "WIN32_LEAN_AND_MEAN"
            "NOMINMAX"
    )
endif()

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        "$<$<CONFIG:Debug>:PRISMSURFACE_DEBUG>"
        "$<$<CONFIG:Release>:PRISMSURFACE_RELEASE>"
)

target_precompile_headers("${PROJECT_NAME}" PRIVATE "pch.h")

set_target_properties("${PROJECT_NAME}" PROPERTIES FOLDER "${PROJECT_NAME}")
set_target_properties("${PROJECT_NAME}" PROPERTIES DEFINE_SYMBOL "PRISMSURFACE_EXPORTS") # Fix uppercase typo in the define symbol

if (MSVC AND PRISM_FORCE_ERRORS)
    target_compile_options("${PROJECT_NAME}" PRIVATE /W4 /WX)
elseif(PRISM_FORCE_ERRORS)
    target_compile_options("${PROJECT_NAME}" PRIVATE -Wall -Wextra -Werror)
endif()

include(GenerateExportHeader)

generate_export_header("${PROJECT_NAME}"
    BASE_NAME PRISM
    EXPORT_MACRO_NAME PRISM_API
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/config/PrismSurface.h"
)

install(TARGETS "${PROJECT_NAME}"
    EXPORT PrismSurfaceTargets
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "include"
    FILES_MATCHING PATTERN "*.h"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/config/"
    DESTINATION "config"
    FILES_MATCHING PATTERN "*.h"
)

if (USE_SOURCE_HEADERS)
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        DESTINATION "src"
        FILES_MATCHING PATTERN "*.h"
    )
endif()
