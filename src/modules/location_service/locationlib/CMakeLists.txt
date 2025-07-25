set(KIT_NAME "kit_location")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE SRC_BASE 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_BASE})
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_log
)