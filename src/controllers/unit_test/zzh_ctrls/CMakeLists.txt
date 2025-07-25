
set(CTL_NAME "ctl_zzhtest")
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
    res/qml/engineeringvehicle_qml/radar_page/radar_page.qml
    res/qml/engineeringvehicle_qml/egv_task_page/egv_task_page.qml
    res/qml/engineeringvehicle_qml/main.qml
)

file(GLOB_RECURSE RES_FILES RELATIVE ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/res/icon/*)
