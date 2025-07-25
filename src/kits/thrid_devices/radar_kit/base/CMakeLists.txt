set(KIT_NAME "kit_radar_base")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE SRC_BASE 
${CMAKE_CURRENT_SOURCE_DIR}/../base/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/../base/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_BASE})
target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
)