set(MODULE_NAME "mod_radar_offline_service")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB_RECURSE CURRENT_RADAR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MODULE_NAME}  OBJECT ${CURRENT_RADAR_SRC})
target_sources(${MODULE_NAME}  PUBLIC
$<TARGET_OBJECTS:kit_module_base>
$<TARGET_OBJECTS:kit_database>

# 选择radar kits
$<TARGET_OBJECTS:kit_radar_base>
$<TARGET_OBJECTS:kit_radar_sick>
# 选择radar kits


)
target_include_directories(${MODULE_NAME}  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)       

target_link_libraries(${MODULE_NAME}  PUBLIC
    shared_dependencies
    kit_module_base
    kit_log
    kit_database

    # 选择radar kits
    kit_radar_base
    kit_radar_sick
    # 选择radar kits
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})