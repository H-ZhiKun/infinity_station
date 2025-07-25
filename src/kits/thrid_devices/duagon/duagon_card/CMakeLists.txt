enable_language(C)  # 显式启用 C 语言支持，无需重复 project()
set(KIT_NAME "kit_duagon_card")
message("---------- Building ${KIT_NAME}  ----------")
message(STATUS "C compiler: ${CMAKE_C_COMPILER}")
file(GLOB_RECURSE HEADERS *.h)
file(GLOB_RECURSE SOURCES *.cpp)

file(GLOB_RECURSE BOARD_SOURCES  third_party/common/src/*.c third_party/target/src/*.c)
file(GLOB_RECURSE REMOVE_SRC third_party/common/src/tcn_bls.c third_party/target/src/osl_par.c)
list(REMOVE_ITEM BOARD_SOURCES ${REMOVE_SRC})
list(APPEND SOURCES ${BOARD_SOURCES})
# message(${SOURCES})

add_compile_options(/wd4311) 
add_compile_options(/wd4312)
add_compile_options(/wd4477)

add_library(${KIT_NAME} SHARED ${HEADERS} ${SOURCES})
target_include_directories(${KIT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/common/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/target/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/pthread_win64/inc)

target_compile_definitions(${KIT_NAME} PRIVATE DUAGON_API_EXPORT)


target_link_libraries(${KIT_NAME} PUBLIC
        kit_log
        setupapi
)
# 输出路径设置
set_target_properties(${KIT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
