file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

message("---------- Building kit_jihe_yanyan_mfc  ----------")

set(KIT_NAME "kit_jihe_yanyan_mfc")

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

# target_sources(${KIT_NAME} PUBLIC 
#     $<TARGET_OBJECTS:kit_database>
# )

target_include_directories(${KIT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} 
    ${THIRD_PARTY_INCLUDE}/JCWLib/
)    

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_log
)

add_custom_target(copy_dll_KIT_NAME ALL
COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/JCWLib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_dependencies(${PROJECT_NAME} copy_dll_KIT_NAME)

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_KIT_NAME)

if(WIN32)

target_link_libraries(${KIT_NAME}  PUBLIC 
    ${THIRD_PARTY_LIB}/JCWLib/JCWLib_x64.lib
)

elseif(UNIX)
# message("UNIX")

endif()

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_KIT_NAME)