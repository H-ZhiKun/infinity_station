# add_subdirectory(camera_service)
set(KIT_NAME "kit_mod_ffmpeg_usb")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
                                     ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

target_include_directories(${KIT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${FFMPEG_INCLUDE_DIRS}  
)       

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_log
)