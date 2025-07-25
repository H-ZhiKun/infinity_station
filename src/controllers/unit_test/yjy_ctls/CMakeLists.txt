message("---------- Building ctl_yjy  ----------")

file(GLOB_RECURSE CURRENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)


find_package(OpenCV REQUIRED COMPONENTS core cudaarithm dnn)


add_library(ctl_yjy OBJECT ${CURRENT_SRC})
 
target_link_libraries(ctl_yjy PUBLIC shared_dependencies kit_cuda ${OpenCV_LIBS})

target_sources(${LIB_CONTROLLERS} PUBLIC 
$<TARGET_OBJECTS:ctl_yjy>
)     
target_link_libraries(${LIB_CONTROLLERS} PUBLIC ctl_yjy)



# 用于选择加载的资源和界面文件 begin
# set(RES_QML
#     res/qml/engineering_vehicle_qml/egv_location_page/egv_location_page.qml
#     res/qml/engineering_vehicle_qml/egv_task_page/egv_task_page.qml
#     res/qml/engineering_vehicle_qml/rfid_page/rfid_page.qml
#     res/qml/engineering_vehicle_qml/main.qml
#     res/qml/engineering_vehicle_qml/egv_video_page/egv_video_page.qml
#     res/qml/public_controls/CustomTitleBar.qml
#     res/qml/public_controls/Table.qml
#     res/qml/public_controls/Modules.qml
#     res/qml/public_controls/VideoDisplay.qml
#     res/qml/public_controls/HighFrequencyChart.qml
# )

# file(GLOB_RECURSE RES_FILES RELATIVE ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/res/icon/*)