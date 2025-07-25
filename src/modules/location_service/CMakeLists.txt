set(MODULE_NAME "mod_location_service")
message("---------- Building ${MODULE_NAME}  ----------")
add_subdirectory(locationlib)

file(GLOB SRC_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                                ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(${MODULE_NAME} OBJECT ${SRC_LOCATION})

target_sources(${MODULE_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_module_base>
$<TARGET_OBJECTS:kit_database>
$<TARGET_OBJECTS:kit_location>
)

target_link_libraries(${MODULE_NAME} PUBLIC
kit_module_base
shared_dependencies
kit_log
kit_database
kit_location
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})