# PrismSurface examples cmake

set(TARGET_NAME "Examples")

file (GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_executable("${TARGET_NAME}" "${SOURCES}")

target_include_directories("${TARGET_NAME}"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries("${TARGET_NAME}"
    PRIVATE
        "${PROJECT_NAME}"
)

set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "${PROJECT_NAME}/Examples")
set_property(DIRECTORY "${CMAKE_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${TARGET_NAME}")
