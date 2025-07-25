
set(KIT_NAME "kit_radar_sick")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)



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

add_custom_target(copy_dll ALL
COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/sick_radar ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
add_dependencies(${PROJECT_NAME} copy_dll)

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_RADAR_SICK)

if(WIN32)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")

    target_link_libraries(${KIT_NAME}  PUBLIC 
        ${THIRD_PARTY_LIB}/sick_radar/SICK_LidarSensor_x64_120d.lib
    )
else()

    target_link_libraries(${KIT_NAME}  PUBLIC 
        ${THIRD_PARTY_LIB}/sick_radar/SICK_LidarSensor_x64_120.lib
    )

endif()


elseif(UNIX)
# message("UNIX")

endif()

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_RADAR_SICK)