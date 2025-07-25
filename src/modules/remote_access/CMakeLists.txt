
set(MODULE_NAME "mod_remote_access")
message("---------- Building ${MODULE_NAME}  ----------")

# 搜集当前目录下的源文件
file(GLOB CURRENT_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(${MODULE_NAME} OBJECT ${CURRENT_SRC})

target_sources(${MODULE_NAME} PUBLIC 
    $<TARGET_OBJECTS:kit_module_base>
    $<TARGET_OBJECTS:kit_remote>
)

target_link_libraries(${MODULE_NAME} PUBLIC
    shared_dependencies
    kit_module_base
    kit_log
    kit_remote
)

target_sources(${LIB_MODULES} PUBLIC 
    $<TARGET_OBJECTS:${MODULE_NAME}>
)     

target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})