
set(KIT_NAME "kit_jihe_yy_kits")
message("---------- Building ${KIT_NAME}  ----------")
file(GLOB_RECURSE CURRENT_SRC 
${CMAKE_CURRENT_SOURCE_DIR}/../JiHeManagerBase.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/../JiHeManagerBase.h 
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(${KIT_NAME} OBJECT ${CURRENT_SRC})

target_compile_definitions(${KIT_NAME} PUBLIC USE_KIT_JIHE_YY)


target_include_directories(${KIT_NAME} PUBLIC
    ${THIRD_PARTY_INCLUDE}/jcwlib
)
target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    ${THIRD_PARTY_LIB}/jcwlib/JCWLib_x64.lib
)

add_custom_target(copy_dll ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/jcwlib ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)