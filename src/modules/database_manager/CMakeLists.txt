set(MODULE_NAME "mod_database_manager")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB_RECURSE CURRENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                                ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MODULE_NAME} OBJECT ${CURRENT_SRC})
target_sources(${MODULE_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_module_base>
$<TARGET_OBJECTS:kit_csv_to_sql>
)
   
target_link_libraries(${MODULE_NAME} PUBLIC
kit_module_base
kit_csv_to_sql
kit_log
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})