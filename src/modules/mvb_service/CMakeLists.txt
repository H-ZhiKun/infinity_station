


set(PRODUCT_NAME "ningbo8")
# set(PRODUCT_NAME "hangde")

message("---------- Building ${PRODUCT_NAME}  ----------")

# add_subdirectory(mvblib)
add_subdirectory(mvbprocessmsg)


file(GLOB SRC_MVB_SERVICE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(mod_mvb_service OBJECT ${SRC_MVB_SERVICE})

target_sources(mod_mvb_service PUBLIC 
                $<TARGET_OBJECTS:kit_module_base>
                $<TARGET_OBJECTS:mvb_mvbprocessmsg_obj>)

target_include_directories(mod_mvb_service PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/mvbprocessmsg/${PRODUCT_NAME} 
)       

target_link_libraries(mod_mvb_service PRIVATE
    shared_dependencies
    kit_module_base
    mvbbaselib
)
