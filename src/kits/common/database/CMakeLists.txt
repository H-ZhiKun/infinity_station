set(KIT_NAME "kit_database")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

target_link_libraries(${KIT_NAME} PUBLIC
    kit_log
    shared_dependencies
)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_target(copy_postgresql ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/postgresql ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

