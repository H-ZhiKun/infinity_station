
set(KIT_NAME "kit_http_client")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE CURRENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${CURRENT_SRC})


target_link_libraries(${KIT_NAME} PRIVATE
    kit_log
    shared_dependencies
)
