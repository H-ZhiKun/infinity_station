set(MODULE_NAME "mod_jihe_service")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB_RECURSE CURRENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                                ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MODULE_NAME} OBJECT ${CURRENT_SRC})
target_sources(${MODULE_NAME} PRIVATE 
$<TARGET_OBJECTS:kit_module_base>
)
target_include_directories(${MODULE_NAME} PRIVATE 
${CMAKE_CURRENT_SOURCE_DIR} 
)       

target_link_libraries(${MODULE_NAME} PRIVATE
kit_module_base
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})