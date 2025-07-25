
set(KIT_NAME "kit_uniqueid_recv_kits")

file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)


add_library(${KIT_NAME} OBJECT ${HEADERS} ${SOURCES})


target_include_directories(${KIT_NAME}  PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}
${THIRD_PARTY_ROOT}/include/histroy_program_comm
)


target_link_libraries(${KIT_NAME}  PUBLIC  
        shared_dependencies
        kit_log
)


target_link_libraries(${KIT_NAME} PUBLIC 
    ${THIRD_PARTY_LIB}/histroy_program_comm/UniqueIdRecieveDll.lib 
)
# add_custom_target(copy_uniqueiddll ALL
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/histroy_program_comm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
# )


