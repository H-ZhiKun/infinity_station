# 添加头文件
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# 添加源文件
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 添加源文件
set(OBJECT_PROJECT_NAME "histroy_program_comm_service")
add_library(${OBJECT_PROJECT_NAME} OBJECT ${HEADERS} ${SOURCES})


target_include_directories(${OBJECT_PROJECT_NAME}  PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}
${THIRD_PARTY_ROOT}/include/histroy_program_comm
)

target_sources(${OBJECT_PROJECT_NAME} PUBLIC 
        $<TARGET_OBJECTS:kit_uniqueid_recv_kits>
)

target_link_libraries(${OBJECT_PROJECT_NAME}  PUBLIC  
        shared_dependencies
        kit_uniqueid_recv_kits
        kit_log
)

message("THIRD_PARTY_LIB: ${THIRD_PARTY_LIB}")
target_link_libraries(${OBJECT_PROJECT_NAME} PUBLIC 
    ${THIRD_PARTY_LIB}/histroy_program_comm/DBCommunication.lib
    ${THIRD_PARTY_LIB}/histroy_program_comm/mvbcommunication.lib
    ${THIRD_PARTY_LIB}/histroy_program_comm/taskcommunication.lib
    ${THIRD_PARTY_LIB}/histroy_program_comm/UniqueIdRecieveDll.lib
)
add_custom_target(copy_oldcommuicationdll ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/histroy_program_comm ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)


target_sources(${LIB_MODULES} PUBLIC 
    $<TARGET_OBJECTS:histroy_program_comm_service>
)     
target_link_libraries(${LIB_MODULES} PUBLIC histroy_program_comm_service)

