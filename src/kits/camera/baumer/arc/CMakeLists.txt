set(KIT_NAME "kit_camera_baumer_arc")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

target_sources(${KIT_NAME} PUBLIC
    $<TARGET_OBJECTS:kit_utils>
    $<TARGET_OBJECTS:kit_camera_baumer_base>
)

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_CAMERA_BAUMER_ARC)

target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_log
    kit_utils
    kit_camera_baumer_base
    ${THIRD_PARTY_LIB}/baumer_camera/bgapi2_ext_sc.lib 
    ${THIRD_PARTY_LIB}/baumer_camera/bgapi2_genicam.lib 
)
