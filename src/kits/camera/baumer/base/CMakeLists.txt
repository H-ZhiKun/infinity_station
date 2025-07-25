set(KIT_NAME "kit_camera_baumer_base")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
)

add_custom_target(copy_dll_baumer_base ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/baumer_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)