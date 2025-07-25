set(MOD_MANAGER "mod_camera_baumer_manager")
message("---------- Building ${MOD_MANAGER}  ----------")
file(GLOB SRC_BASE 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MOD_MANAGER} OBJECT ${SRC_BASE})
target_link_libraries(${MOD_MANAGER} PUBLIC
    shared_dependencies
    kit_camera_baumer_base
    kit_utils
    kit_camera_tools
    kit_module_base
)

target_sources(${MOD_MANAGER} PUBLIC
    $<TARGET_OBJECTS:kit_camera_baumer_base>
    $<TARGET_OBJECTS:kit_utils>
    $<TARGET_OBJECTS:kit_camera_tools>
    $<TARGET_OBJECTS:kit_module_base>
)

if (TARGET kit_camera_baumer_normal)
    target_link_libraries(${MOD_MANAGER} PUBLIC kit_camera_baumer_normal)
    target_sources(${MOD_MANAGER} PUBLIC $<TARGET_OBJECTS:kit_camera_baumer_normal>)
endif()

if(TARGET kit_camera_baumer_arc)
    target_link_libraries(${MOD_MANAGER} PUBLIC kit_camera_baumer_arc)
    target_sources(${MOD_MANAGER} PUBLIC $<TARGET_OBJECTS:kit_camera_baumer_arc>)
endif()


target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MOD_MANAGER}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MOD_MANAGER})
