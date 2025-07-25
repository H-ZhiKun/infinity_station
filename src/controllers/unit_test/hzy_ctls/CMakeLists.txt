
set(CTL_NAME "ctl_hzytest")
message("---------- Building ${CTL_NAME}  ----------")

file(GLOB_RECURSE CURRENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${CTL_NAME} OBJECT ${CURRENT_SRC})

target_link_libraries(${CTL_NAME} PUBLIC 
shared_dependencies 
kit_log
)
target_sources(${LIB_CONTROLLERS} PUBLIC 
$<TARGET_OBJECTS:${CTL_NAME}>
)     
target_link_libraries(${LIB_CONTROLLERS} PUBLIC ${CTL_NAME})

# 用于选择加载的资源和界面文件 begin
set(RES_QML
    res/qml/engineering_vehicle_qml/radar_page/radar_page.qml
    res/qml/engineering_vehicle_qml/egv_task_page/egv_task_page.qml
    res/qml/engineering_vehicle_qml/egv_video_page/egv_video_page.qml
    res/qml/public_controls/CustomTitleBar.qml
    res/qml/public_controls/Table.qml
    res/qml/engineering_vehicle_qml/rfid_page/rfid_page.qml
    res/qml/public_controls/Modules.qml
    res/qml/public_controls/VideoDisplay.qml
    res/qml/engineering_vehicle_qml/main.qml
    res/qml/engineering_vehicle_qml/mvb_page/mvb_page.qml
    res/qml/public_controls/ConfigEditorMain.qml
    res/qml/engineering_vehicle_qml/egv_location_page/egv_location_page.qml
    res/qml/engineering_vehicle_qml/egv_dropper_trigger_page/egv_dropper_trigger_page.qml
    res/qml/engineering_vehicle_qml/egv_inspectionimage_page/egv_inspectionimage_page.qml
)

file(GLOB_RECURSE RES_FILES RELATIVE ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/res/icon/*)
