set(KIT_NAME "kit_save_video_by_ffmpeg")
message("---------- Building ${KIT_NAME}  ----------")

file(GLOB_RECURSE CURRENT_FFMPEG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${CURRENT_FFMPEG_SRC})

find_package(FFmpeg REQUIRED COMPONENTS avcodec avformat avutil swscale avdevice swresample avfilter)




target_include_directories(${KIT_NAME} PUBLIC  
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${FFMPEG_INCLUDE_DIRS}  
)       

target_link_directories(${KIT_NAME} PUBLIC ${FFMPEG_LIBRARY_DIRS})

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_log
    ${FFMPEG_LIBRARIES} avcodec avformat avutil swscale
)