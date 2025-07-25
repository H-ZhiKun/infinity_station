set(KIT_NAME "kit_camera_service")
message("---------- Building ${KIT_NAME}  ----------")
# 添加头文件
file(GLOB HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# 添加源文件
file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# 添加源文件
add_library(${KIT_NAME} OBJECT ${HEADERS} ${SOURCES})

option(BASLER " " ON)
option(BAUMER " " ON)
option(HIK " " ON)
option(FFMPEG " " ON)


if(LINUX)
    option(BAUMER " " OFF)
endif()

if(BAUMER)
    message("---------- Building kit_baumer_camera ----------")
    add_subdirectory(baumer)
    target_sources(${KIT_NAME} PUBLIC
        $<TARGET_OBJECTS:kit_mod_baumer>
    )
endif()

if(BASLER)
    message("---------- Building kit_basler_camera ----------")
    add_subdirectory(basler)
    target_sources(${KIT_NAME} PUBLIC
        $<TARGET_OBJECTS:kit_mod_basler>
    )
endif()

if(HIK)
    message("---------- Building kit_hik_camera ----------")
    add_subdirectory(hik)
    target_sources(${KIT_NAME} PUBLIC
        $<TARGET_OBJECTS:kit_mod_hik>
    )
endif()

if(FFMPEG)
    message("---------- Building kit_ffmpeg ----------")
    add_subdirectory(ffmpeg)
    target_sources(${KIT_NAME} PUBLIC
        $<TARGET_OBJECTS:kit_mod_ffmpeg_usb>
    )
endif()

# set(FFMPEG_INCLUDE_DIRS "/usr/local/include")
# set(FFMPEG_LIBRARY_DIRS "/usr/local/lib")

find_package(FFmpeg REQUIRED COMPONENTS avcodec avformat avutil swscale avdevice swresample avfilter)

target_include_directories(${KIT_NAME}  PUBLIC 
${FFMPEG_INCLUDE_DIRS} 
${CMAKE_CURRENT_SOURCE_DIR}
${THIRD_PARTY_ROOT}/include/hik_camera 
${THIRD_PARTY_ROOT}/include/baumer_camera
${THIRD_PARTY_ROOT}/include/basler_camera 
)

target_link_directories(${KIT_NAME} PUBLIC ${FFMPEG_LIBRARY_DIRS})

target_link_libraries(${KIT_NAME} PUBLIC 
                    shared_dependencies 
                    kit_log
                    ${FFMPEG_LIBRARIES} avcodec avformat avutil swscale)

if(WIN32)

    if(BASLER)
        target_link_libraries(${KIT_NAME} PRIVATE 
            ${THIRD_PARTY_LIB}/basler_camera/GCBase_MD_VC141_v3_1_Basler_pylon.lib
            ${THIRD_PARTY_LIB}/basler_camera/GenApi_MD_VC141_v3_1_Basler_pylon.lib
            ${THIRD_PARTY_LIB}/basler_camera/PylonBase_v6_3.lib
            ${THIRD_PARTY_LIB}/basler_camera/PylonC.lib
            ${THIRD_PARTY_LIB}/basler_camera/PylonGUI_v6_3.lib
            ${THIRD_PARTY_LIB}/basler_camera/PylonUtility_v6_3.lib
        )
        add_custom_target(copy_cameradll ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/basler_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
    endif()

    if(BAUMER)
        target_link_libraries(${KIT_NAME} PRIVATE 
            ${THIRD_PARTY_LIB}/baumer_camera/bgapi2_genicam.lib
        )
        add_custom_target(copy_baumerCameradll ALL
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/baumer_camera ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
    endif()

    if(HIK)
        target_link_libraries(${KIT_NAME} PRIVATE 
            ${THIRD_PARTY_LIB}/hik_camera/PlayCtrl.lib
            ${THIRD_PARTY_LIB}/hik_camera/HCNetSDK.lib
        )
       
    endif()    

elseif(UNIX)
# message("UNIX")
    if(HIK)
        target_link_libraries(${KIT_NAME} PRIVATE
            ${THIRD_PARTY_LIB}/libhcnetsdk.so
            ${THIRD_PARTY_LIB}/libPlayCtrl.so
            # src/modules/video_cap/detect_kit/panto/lib/libCorePipleline.so
            # src/modules/video_cap/detect_kit/panto/lib/libJC23-05_ningBo_5CPantographDetection.so
        )
    endif()


endif()

