set(KIT_NAME "kit_rfid_vfr61m")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
message("---------- Building ${KIT_NAME}  ----------")
add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_RFID_VFR61M)

target_sources(${KIT_NAME} PUBLIC 
    $<TARGET_OBJECTS:kit_tcp_client>
    $<TARGET_OBJECTS:kit_udp>
    $<TARGET_OBJECTS:kit_read_csv>
    $<TARGET_OBJECTS:kit_rfid_base>
)
target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_log
    kit_udp
    kit_tcp_client
    kit_read_csv
    kit_rfid_base
)