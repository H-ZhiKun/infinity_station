message("---------- Building ctl_hzk  ----------")

file(GLOB_RECURSE CURRENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(Qt6 6.8 REQUIRED COMPONENTS HttpServer)
find_package(OpenCV REQUIRED COMPONENTS core cudaarithm dnn)


add_library(ctl_hzk OBJECT ${CURRENT_SRC})
target_sources(ctl_hzk PUBLIC 
$<TARGET_OBJECTS:kit_database>
)         
target_link_libraries(ctl_hzk PUBLIC shared_dependencies Qt6::HttpServer kit_database ${OpenCV_LIBS})

target_sources(${LIB_CONTROLLERS} PUBLIC 
$<TARGET_OBJECTS:ctl_hzk>
)     
target_link_libraries(${LIB_CONTROLLERS} PUBLIC ctl_hzk)

set(RES_QML
    res/qml/engineering_vehicle_qml/egv_location_page/egv_location_page.qml
    res/qml/engineering_vehicle_qml/egv_task_page/egv_task_page.qml
    res/qml/engineering_vehicle_qml/rfid_page/rfid_page.qml
    res/qml/engineering_vehicle_qml/main.qml
    res/qml/engineering_vehicle_qml/egv_video_page/egv_video_page.qml
    res/qml/public_controls/CustomTitleBar.qml
    res/qml/public_controls/Table.qml
    res/qml/public_controls/Modules.qml
    res/qml/public_controls/VideoDisplay.qml
    res/qml/public_controls/HighFrequencyChart.qml
)

file(GLOB_RECURSE RES_FILES RELATIVE ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/res/icon/*)