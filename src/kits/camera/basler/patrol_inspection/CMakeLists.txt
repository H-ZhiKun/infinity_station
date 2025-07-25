set(KIT_NAME "kit_camera_basler_patrol_inspection")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_log
    kit_camera_basler_base
)

target_sources(${KIT_NAME} PRIVATE
$<TARGET_OBJECTS:kit_camera_basler_base>
)


target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_CAMERA_BASLER_PATROL_INSPECTION)

add_custom_target(copy_dll_basler_patrolins ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/basler_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
