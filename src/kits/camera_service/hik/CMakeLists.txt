# add_subdirectory(camera_service)
set(KIT_NAME "kit_mod_hik")
file(GLOB CURRENT_HIK ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${CURRENT_HIK})

target_include_directories(${KIT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${THIRD_PARTY_ROOT}/include/hik_camera 
)       

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_log
)

add_custom_target(copy_dll_hik_base ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/hik_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
