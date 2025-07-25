


# set(PRODUCT_NAME "ningbo8")
# set(PRODUCT_NAME "hangde")

# message("---------- Building ${PRODUCT_NAME}  ----------")

set(MODULE_NAME "mod_temperature_service")


file(GLOB SRC_MODULE_SERVICE ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                          ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MODULE_NAME} OBJECT ${SRC_MODULE_SERVICE})

target_sources(${MODULE_NAME} PUBLIC 
                $<TARGET_OBJECTS:kit_module_base>
                $<TARGET_OBJECTS:kit_camera_hik_temperature>)

target_include_directories(${MODULE_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}
)       

target_link_libraries(${MODULE_NAME} PRIVATE
    shared_dependencies
    kit_module_base
    kit_camera_hik_temperature
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})