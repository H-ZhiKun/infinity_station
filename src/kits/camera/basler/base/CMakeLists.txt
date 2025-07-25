set(KIT_NAME "kit_camera_basler_base")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
)

target_include_directories(${KIT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${THIRD_PARTY_ROOT}/include/basler_camera
)       

target_link_libraries(${KIT_NAME} PUBLIC 
    ${THIRD_PARTY_LIB}/basler_camera/GCBase_MD_VC141_v3_1_Basler_pylon.lib
    ${THIRD_PARTY_LIB}/basler_camera/GenApi_MD_VC141_v3_1_Basler_pylon.lib
    ${THIRD_PARTY_LIB}/basler_camera/PylonBase_v6_3.lib
    ${THIRD_PARTY_LIB}/basler_camera/PylonC.lib
    ${THIRD_PARTY_LIB}/basler_camera/PylonGUI_v6_3.lib
    ${THIRD_PARTY_LIB}/basler_camera/PylonUtility_v6_3.lib
)


add_custom_target(copy_dll_basler_base ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/basler_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
