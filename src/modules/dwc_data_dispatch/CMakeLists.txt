
set(MODULE_NAME "mod_Dwc_Data_Dispatch")
message("---------- Building ${MODULE_NAME}  ----------")
file(GLOB SRC_DWC_DATA_DISPATCH ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
                                ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_library(${MODULE_NAME} OBJECT ${SRC_DWC_DATA_DISPATCH})


target_sources(${MODULE_NAME} PUBLIC 
$<TARGET_OBJECTS:kit_module_base>
)  

target_link_libraries(${MODULE_NAME} PUBLIC
     kit_module_base
     shared_dependencies
     kit_log
)


target_sources(${LIB_MODULES} PUBLIC 
$<TARGET_OBJECTS:${MODULE_NAME}>
)     
target_link_libraries(${LIB_MODULES} PUBLIC ${MODULE_NAME})