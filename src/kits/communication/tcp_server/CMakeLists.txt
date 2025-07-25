
set(KIT_NAME "kit_tcp_server")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_include_directories(${KIT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} 
)       
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_log
)
