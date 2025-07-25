set(CONTROLER_NAME "ctl_droppertriggerpage")

message("---------- Building ${CONTROLER_NAME}  ----------")

file(GLOB_RECURSE DROPPERTRIGGERPAGE_CTL ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
                          
add_library(${CONTROLER_NAME} OBJECT ${DROPPERTRIGGERPAGE_CTL})

target_sources(${CONTROLER_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_controller_base>
)     
target_link_libraries(${CONTROLER_NAME} PUBLIC 
kit_controller_base
)

target_sources(${LIB_CONTROLLERS} PUBLIC 
$<TARGET_OBJECTS:${CONTROLER_NAME}>
)     
target_link_libraries(${LIB_CONTROLLERS} PUBLIC ${CONTROLER_NAME})