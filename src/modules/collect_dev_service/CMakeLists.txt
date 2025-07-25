
message("---------- Building collect_dev_service  ----------")
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(collect_dev_service
 OBJECT ${HEADERS} ${SOURCES})

target_sources(collect_dev_service PUBLIC
$<TARGET_OBJECTS:kit_module_base>
)

target_include_directories(collect_dev_service  PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(collect_dev_service PUBLIC
    shared_dependencies
    kit_module_base
    kit_log
)


