set(KIT_NAME "kit_camera_hik_base")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
)

target_include_directories(${KIT_NAME} PUBLIC  ${THIRD_PARTY_INCLUDE}/hik_temperature/ )

add_custom_target(copy_dll_hik_base ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/hik_temperature ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
