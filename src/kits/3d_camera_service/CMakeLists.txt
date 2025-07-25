
set(KIT_NAME "kit_3d_camera_service")
message("---------- Building kit_3d_camera_service  ----------")
file(GLOB_RECURSE SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_sources(${KIT_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_module_base>
)

target_include_directories(${KIT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}/factory
)  

target_link_libraries(${KIT_NAME} PUBLIC
    kit_module_base
)
