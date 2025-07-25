cmake_minimum_required(VERSION 3.21)
# 设置项目信息
project(Corrugationbaselib VERSION 1.0)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty)


add_definitions(-DCorrugationBASELIB_API_EXPORTS)

# 添加头文件
file(GLOB_RECURSE HEADERS src/*.h)

# 添加源文件
file(GLOB_RECURSE SOURCES src/*.cpp thirdparty/common/src/*.c thirdparty/target/src/*.c)

file(GLOB_RECURSE REMOVE_SRC thirdparty/common/src/tcn_bls.c thirdparty/target/src/osl_par.c)

#排查指定文件
list(REMOVE_ITEM SOURCES ${REMOVE_SRC})
find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
#find_library(LIBPTHEADER_PATH pthreadVC2 ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/pthread_win64/lib)
# IF(NOT LIBPTHEADER_PATH)
# MESSAGE(FATAL_ERROR "pthreadVC2 not found")
# ENDIF(NOT LIBPTHEADER_PATH) 

#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/pthread_win64/lib)


#add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
#message("****${LIBPTHEADER_PATH}")
add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
            spdlog::spdlog
            yaml-cpp::yaml-cpp
            ${LIBPTHEADER_PATH}
            Setupapi)

# 输出路径设置
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
