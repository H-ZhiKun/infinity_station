set(KIT_NAME "kit_connectivity_base")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_sources(${KIT_NAME} PUBLIC 
    $<TARGET_OBJECTS:kit_utils>
)

target_link_libraries(${KIT_NAME} PRIVATE shared_dependencies kit_utils)
