
set(KIT_NAME "kit_kafka")
message("---------- Building kit_kafka  ----------")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
find_package(RdKafka CONFIG REQUIRED)
add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})



target_include_directories(${KIT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} 
)       
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_log
    RdKafka::rdkafka RdKafka::rdkafka++
)
