
set(MVB_PROCESSMSG_OBJECT_NAME "mvb_mvbprocessmsg_obj")

file(GLOB MVB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

file(GLOB PRODUCT_SRC    ${CMAKE_CURRENT_SOURCE_DIR}/${PRODUCT_NAME}/*.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/${PRODUCT_NAME}/*.h )

list(APPEND MVB_SRC ${PRODUCT_SRC})
add_library(${MVB_PROCESSMSG_OBJECT_NAME} OBJECT ${MVB_SRC})
target_include_directories(${MVB_PROCESSMSG_OBJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} 
)     

target_link_libraries(${MVB_PROCESSMSG_OBJECT_NAME} PUBLIC
    shared_dependencies)
