
set(KIT_NAME "kit_radar_YanYan")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

message("---------- Building kit_radar_YanYan  ----------")



add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

target_sources(${KIT_NAME} PUBLIC 
    $<TARGET_OBJECTS:kit_database>
)

target_include_directories(${KIT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} 
)    

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_database
    kit_log
)

add_custom_target(copy_dll_yanyan ALL
COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/zeroMQ ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_dependencies(${PROJECT_NAME} copy_dll_yanyan)

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_RADAR_YanYan)

if(WIN32)

target_link_libraries(${KIT_NAME}  PUBLIC 
    ${THIRD_PARTY_LIB}/zeroMQ/libzmq-v143-mt-4_3_6.lib
)

elseif(UNIX)
# message("UNIX")

endif()

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_RADAR_YANYAN)