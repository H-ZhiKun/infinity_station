set(KIT_NAME "kit_udp_6017")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

target_sources(${KIT_NAME} PUBLIC 
    $<TARGET_OBJECTS:kit_udp>
    $<TARGET_OBJECTS:kit_utils>
)

target_include_directories(${KIT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} 
)

target_link_libraries(${KIT_NAME} PRIVATE
    kit_log
    kit_udp
    kit_utils
    shared_dependencies
)
