set(KIT_NAME "kit_csv_to_sql")

message("---------- Building ${KIT_NAME}  ----------")
file(GLOB CURRENT_CSVTOSQL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${CURRENT_CSVTOSQL_SRC})
target_sources(${KIT_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_database>
$<TARGET_OBJECTS:kit_read_csv>
)
target_include_directories(${KIT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR} 
)       

target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_database
    kit_read_csv
    kit_log
)
