# PrismSurface windows example cmake

set(TARGET_NAME "WindowExample")

file (GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable("${TARGET_NAME}" "${SOURCES}")

target_include_directories("${TARGET_NAME}"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries("${TARGET_NAME}"
    PRIVATE
        "${PROJECT_NAME}"
)

set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "${PROJECT_NAME}/Examples")
set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${TARGET_NAME}")
