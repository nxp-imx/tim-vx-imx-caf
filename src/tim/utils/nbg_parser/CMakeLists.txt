message("src/tim/vx/utils/nbg_parser")

set(TARGET_NAME "nbg_parser")

aux_source_directory(. ${TARGET_NAME}_SRCS)
add_library(${TARGET_NAME} STATIC ${${TARGET_NAME}_SRCS})
target_include_directories(${TARGET_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)