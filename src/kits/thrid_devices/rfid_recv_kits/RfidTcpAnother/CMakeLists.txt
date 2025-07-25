
# file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

# if(NOT TARGET kit_rfid_base)
#     set(KIT_BASE "kit_rfid_base")
#     message("---------- Building ${KIT_BASE}  ----------")
#     file(GLOB_RECURSE SRC_BASE 
#     ${CMAKE_CURRENT_SOURCE_DIR}/../RfidBase/*.cpp 
#     ${CMAKE_CURRENT_SOURCE_DIR}/../RfidBase/*.h)

#     add_library(${KIT_BASE} OBJECT ${SRC_BASE})
#     target_link_libraries(${KIT_BASE} PRIVATE
#         shared_dependencies
#     )
# endif()

# set(RFID_RECV_ANOTHER_TCP "kit_rfid_another_tcp")
# message("---------- Building ${RFID_RECV_ANOTHER_TCP}  ----------")

# target_compile_definitions(${RFID_RECV_ANOTHER_TCP} PUBLIC USE_KIT_RFID_ANOHTER_TCP)

# add_library(${RFID_RECV_ANOTHER_TCP} OBJECT ${SRC_CURRENT})

# target_sources(${RFID_RECV_ANOTHER_TCP} PUBLIC 
#     $<TARGET_OBJECTS:kit_tcp>
# )
# target_include_directories(${RFID_RECV_ANOTHER_TCP} PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR} 
# )       
# target_link_libraries(${RFID_RECV_ANOTHER_TCP} PUBLIC
#     shared_dependencies
#     kit_log
#     kit_tcp
# )
set(KIT_NAME "kit_rfid_another_tcp")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

 target_sources(${KIT_NAME} PUBLIC 
    $<TARGET_OBJECTS:kit_tcp>
    $<TARGET_OBJECTS:kit_rfid_base>
)

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_log
    kit_tcp
    kit_rfid_base
)
