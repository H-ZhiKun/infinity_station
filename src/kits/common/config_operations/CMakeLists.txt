set(KIT_NAME "kit_config_operations")

message("---------- Building ${KIT_NAME}  ----------")
file(GLOB CURRENT_CONFIGOPERATIONS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${CURRENT_CONFIGOPERATIONS_SRC})

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_log
)
