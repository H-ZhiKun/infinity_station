
set(MODULE_NAME "mod_jihetrigger_service")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB SRC_JIHETRIGGER_SERVICE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(JIHETRIGGER_PORJECT_NAME "mod_jihetrigger_service")

add_library(${MODULE_NAME} OBJECT ${SRC_JIHETRIGGER_SERVICE})

target_sources(${MODULE_NAME} PUBLIC
        $<TARGET_OBJECTS:kit_module_base>

)

target_include_directories(${MODULE_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)       

target_link_libraries(${MODULE_NAME} PRIVATE
    shared_dependencies

)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})