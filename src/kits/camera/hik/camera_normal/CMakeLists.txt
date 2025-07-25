
set(KIT_NAME "kit_camera_hik_normal")
message("---------- Building ${KIT_NAME}  ----------")

file(GLOB_RECURSE SRC_CURRENT 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

target_compile_definitions(${KIT_NAME} PUBLIC 
    USE_KIT_CAMERA_HIK_NORMAL
    
)

target_sources(${KIT_NAME} PRIVATE
    $<TARGET_OBJECTS:kit_utils>
    $<TARGET_OBJECTS:kit_camera_hik_base>
)

find_package(TBB CONFIG REQUIRED)
target_link_libraries(${KIT_NAME} PRIVATE TBB::tbb)

find_package(OpenCV REQUIRED COMPONENTS core cudaarithm dnn cudaimgproc)
target_link_libraries(${KIT_NAME} PUBLIC ${OpenCV_LIBS})

if(LINUX)
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_camera_hik_base
    kit_log
    kit_utils
    kit_cuda
    ${THIRD_PARTY_BIN}/hik_temperature/libPlayCtrl.so
    ${THIRD_PARTY_BIN}/hik_temperature/libhcnetsdk.so
)

else()
target_link_libraries(${KIT_NAME} PRIVATE
    shared_dependencies
    kit_camera_hik_base
    kit_log
    kit_cuda
    kit_utils
    ${THIRD_PARTY_LIB}/hik_temperature/PlayCtrl.lib
    ${THIRD_PARTY_LIB}/hik_temperature/HCNetSDK.lib
)
endif()