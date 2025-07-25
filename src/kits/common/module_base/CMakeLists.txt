
set(KIT_NAME "kit_module_base")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_sources(${KIT_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_invoke>
)         
target_include_directories(${KIT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} 
)       
target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_invoke
)
