
set(KIT_NAME "kit_camera_hik_nvr")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE CURRENT_SRC 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${CURRENT_SRC})

target_sources(${KIT_NAME} PRIVATE
    $<TARGET_OBJECTS:kit_log>
    $<TARGET_OBJECTS:kit_utils>
)

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_CAMERA_HIK_NORMAL)

target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_log
    kit_utils
    ${THIRD_PARTY_LIB}/hik_camera/PlayCtrl.lib
    ${THIRD_PARTY_LIB}/hik_camera/HCNetSDK.lib
)

add_custom_target(hik_nvr_dll ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/hik_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

