message("samples/lenet_lite")

set(TARGET_NAME "lenet_lite")

aux_source_directory(. ${TARGET_NAME}_SRCS)
add_executable(${TARGET_NAME} ${${TARGET_NAME}_SRCS})

target_link_libraries(${TARGET_NAME} PRIVATE tim-vx)
target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)