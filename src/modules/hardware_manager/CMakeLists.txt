set(MODULE_NAME "mod_hardware_manager")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB_RECURSE DEV_INFO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                                ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${MODULE_NAME} OBJECT ${DEV_INFO_SRC})
target_sources(${MODULE_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_module_base>
)

if (WIN32)
    target_sources(${MODULE_NAME} PUBLIC 
        $<TARGET_OBJECTS:kit_hw_windows>
    )

    target_link_libraries(${MODULE_NAME} PRIVATE
        kit_hw_windows
    )
endif()

target_link_libraries(${MODULE_NAME} PRIVATE
kit_module_base
)

target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})