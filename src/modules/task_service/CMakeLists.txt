
set(MODULE_NAME "mod_Task_service")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB SRC_TASK_SERVICE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(TASK_PORJECT_NAME "mod_Task_service")

add_library(${TASK_PORJECT_NAME} OBJECT ${SRC_TASK_SERVICE})

target_sources(${TASK_PORJECT_NAME} PUBLIC
        $<TARGET_OBJECTS:kit_module_base>
        $<TARGET_OBJECTS:kit_database>
        $<TARGET_OBJECTS:kit_read_csv>  
        $<TARGET_OBJECTS:kit_utils>  
)


target_link_libraries(${TASK_PORJECT_NAME} PUBLIC
    shared_dependencies
    kit_module_base
    kit_database
    kit_read_csv
    kit_utils
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})
