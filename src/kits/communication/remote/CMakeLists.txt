
set(KIT_NAME "kit_remote")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

find_package(cppzmq CONFIG REQUIRED)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})
target_include_directories(${KIT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} 
)       
target_link_libraries(${KIT_NAME} PUBLIC
    kit_log
    cppzmq cppzmq-static
)
