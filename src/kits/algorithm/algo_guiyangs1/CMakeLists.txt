set(KIT_NAME "kit_algo_guiyangs1")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${SRC_CURRENT})

  

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    kit_log
    ${THIRD_PARTY_LIB}/guiyangs1_temp_algo/DetectionRecognition.lib
    ${THIRD_PARTY_LIB}/guiyangs1_temp_algo/CorePipleline.lib
    ${THIRD_PARTY_LIB}/guiyangs1_temp_algo/JC24_04Mexico_MonorailAOI.lib

)


add_custom_target(copy_algoDll ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/guiyangs1_temp_algo ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)



