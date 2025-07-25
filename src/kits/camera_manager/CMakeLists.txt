set(KIT_NAME "kit_camera_manager")
file(GLOB_RECURSE SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(FFmpeg REQUIRED COMPONENTS avcodec avformat avutil swscale avdevice swresample avfilter)

# 添加源文件
add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})


target_include_directories(${KIT_NAME}  PUBLIC 
${FFMPEG_INCLUDE_DIRS} 
${CMAKE_CURRENT_SOURCE_DIR}
${THIRD_PARTY_ROOT}/include/hik_camera
${THIRD_PARTY_ROOT}/include/baumer_camera
)


target_link_directories(${KIT_NAME}  PUBLIC ${FFMPEG_LIBRARY_DIRS})

target_link_libraries(${KIT_NAME}  PUBLIC 
                    ${FFMPEG_LIBRARIES} avcodec avformat avutil swscale shared_dependencies)

if(WIN32)
# message("WIN32")
    target_link_libraries(${KIT_NAME} PUBLIC 
        ${THIRD_PARTY_LIB}/hik_camera/PlayCtrl.lib
        ${THIRD_PARTY_LIB}/hik_camera/HCNetSDK.lib
        ${THIRD_PARTY_LIB}/baumer_camera/bgapi2_genicam.lib
    )
    add_custom_target(copy_cameradll ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/hik_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/baumer_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

elseif(UNIX)
# message("UNIX")
    target_link_libraries(${KIT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libhcnetsdk.so
        ${PROJECT_SOURCE_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libPlayCtrl.so
        src/modules/video_cap/detect_kit/panto/lib/libCorePipleline.so
        src/modules/video_cap/detect_kit/panto/lib/libJC23-05_ningBo_5CPantographDetection.so
    )
endif()
