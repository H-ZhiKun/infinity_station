set(MODULE_NAME "mod_pressure_spot_service")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB_RECURSE SRC_PRESSURE_SPOT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                                    ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MODULE_NAME} OBJECT ${SRC_PRESSURE_SPOT})
target_sources(${MODULE_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_module_base>
$<TARGET_OBJECTS:kit_udp>
)
target_link_libraries(${MODULE_NAME} PRIVATE
    shared_dependencies
    kit_module_base
    kit_udp
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})
