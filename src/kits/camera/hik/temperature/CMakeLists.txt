
set(KIT_NAME "kit_camera_hik_temperature")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE SRC_CURRENT 
# ${CMAKE_CURRENT_SOURCE_DIR}/../CameraManagerBase.cpp 
# ${CMAKE_CURRENT_SOURCE_DIR}/../CameraManagerBase.h 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

# target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_CAMERA_BAUMER)

target_include_directories(${KIT_NAME} PUBLIC  ${THIRD_PARTY_INCLUDE}/hik_temperature/ )



message(${THIRD_PARTY_LIB})
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_log
    ${THIRD_PARTY_LIB}/hik_temperature/GdiPlus.lib 
    ${THIRD_PARTY_LIB}/hik_temperature/HCCore.lib 
    ${THIRD_PARTY_LIB}/hik_temperature/HCNetSDK.lib 
    ${THIRD_PARTY_LIB}/hik_temperature/PlayCtrl.lib 

  

)

# add_custom_target(copy_dll ALL
#     COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/hik_temperature ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
# )