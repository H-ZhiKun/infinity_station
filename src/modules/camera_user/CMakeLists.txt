set(MODULE_NAME "mod_camera_user")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB CURRENT_CAMERA_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MODULE_NAME} OBJECT ${CURRENT_CAMERA_SRC})
target_sources(${MODULE_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_module_base>
$<TARGET_OBJECTS:kit_camera_service>
)

target_include_directories(${MODULE_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR} 
)       

target_link_libraries(${MODULE_NAME} PUBLIC
    shared_dependencies
    kit_module_base
    kit_camera_service
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})