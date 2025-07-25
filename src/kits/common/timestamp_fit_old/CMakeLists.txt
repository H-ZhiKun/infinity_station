set(KIT_NAME "kit_timestamp_fit_old")
message("---------- Building ${KIT_NAME}  ----------")

file(GLOB SRC_CURRENT 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

target_include_directories(${KIT_NAME} 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${KIT_NAME} 
    PUBLIC
        shared_dependencies
)