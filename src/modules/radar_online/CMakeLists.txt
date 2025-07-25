set(MODULE_NAME "mod_radar_online_service")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB_RECURSE CURRENT_RADAR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(TBB CONFIG REQUIRED)

add_library(${MODULE_NAME}  OBJECT ${CURRENT_RADAR_SRC})
target_sources(${MODULE_NAME}  PUBLIC
$<TARGET_OBJECTS:kit_module_base>
$<TARGET_OBJECTS:kit_database>
$<TARGET_OBJECTS:kit_radar_base>
)

target_link_libraries(${MODULE_NAME} PRIVATE TBB::tbb)

target_include_directories(${MODULE_NAME}  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)       

target_link_libraries(${MODULE_NAME}  PUBLIC
    shared_dependencies
    kit_module_base
    kit_log
    kit_database
    kit_radar_base
)

if (TARGET kit_radar_sick)
    target_link_libraries(${MODULE_NAME} PUBLIC kit_radar_sick)
    target_sources(${MODULE_NAME} PUBLIC $<TARGET_OBJECTS:kit_radar_sick>)
endif()

if(TARGET kit_radar_YanYan)
    target_link_libraries(${MODULE_NAME} PUBLIC kit_radar_YanYan)
    target_sources(${MODULE_NAME} PUBLIC $<TARGET_OBJECTS:kit_radar_YanYan>)
endif()

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})

