set(MOD_MANAGER "mod_camera_basler_manager")
message("---------- Building ${MOD_MANAGER}  ----------")
file(GLOB SRC_BASE 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MOD_MANAGER} OBJECT ${SRC_BASE})
target_link_libraries(${MOD_MANAGER} PUBLIC
    shared_dependencies
    kit_camera_basler_base
    kit_utils
    kit_camera_tools
    kit_module_base
    kit_camera_basler_trigger
    kit_camera_basler_patrol_inspection
)

target_sources(${MOD_MANAGER} PUBLIC
    $<TARGET_OBJECTS:kit_camera_basler_base>
    $<TARGET_OBJECTS:kit_utils>
    $<TARGET_OBJECTS:kit_camera_tools>
    $<TARGET_OBJECTS:kit_module_base>
    $<TARGET_OBJECTS:kit_camera_basler_trigger>
    $<TARGET_OBJECTS:kit_camera_basler_patrol_inspection>
)



target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MOD_MANAGER}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MOD_MANAGER})
