set(KIT_NAME "kit_cuda")
message("---------- Building ${KIT_NAME} ----------")

enable_language(CUDA)
find_package(CUDAToolkit REQUIRED)

file(GLOB_RECURSE CURRENTSRC ${CMAKE_CURRENT_SOURCE_DIR}/*.h
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file(GLOB SRC_CUDA ${CMAKE_CURRENT_SOURCE_DIR}/*.cu
${CMAKE_CURRENT_SOURCE_DIR}/*.cuh
)

add_library(${KIT_NAME} SHARED ${SRC_CUDA} ${CURRENTSRC})

target_compile_definitions(${KIT_NAME} PRIVATE CUDAYV12RGB_EXPORTS)

set_target_properties(${KIT_NAME} PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_RESOLVE_DEVICE_SYMBOLS ON
    CUDA_STANDARD 17
    CUDA_STANDARD_REQUIRED ON
)

target_include_directories(${KIT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CUDAToolkit_INCLUDE_DIRS}
)
target_link_libraries(${KIT_NAME} PUBLIC CUDA::cudart)

# set_target_properties(${KIT_NAME} PROPERTIES
#     CUDA_RUNTIME_LIBRARY "Shared"
# )
if(MSVC)
   if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler=/MDd")
        add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
    else()
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler=/MD") 
        add_compile_definitions(_ITERATOR_DEBUG_LEVEL=2)
    endif()

endif()

# Windows特有配置
if(WIN32)
    # 确保共享库导出符号
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    # 复制CUDA DLL到运行目录（如果需要）
    add_custom_command(TARGET ${KIT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:CUDA::cudart>
            $<TARGET_FILE_DIR:${KIT_NAME}>
    )
endif()

message(STATUS "CUDA Toolkit Version: ${CUDAToolkit_VERSION}")
message(STATUS "CUDA Architectures: ${CMAKE_CUDA_ARCHITECTURES}")
message(STATUS "CUDA Compiler: ${CMAKE_CUDA_COMPILER}")

set(CMAKE_CUDA_FLAGS 
    "${CMAKE_CUDA_FLAGS} 
    -std=c++17 
    --generate-code=arch=compute_75,code=[compute_75,sm_75] 
    -Xcompiler=/EHsc 
    -Xcompiler=/utf-8 
    -Xcompiler=/Zc:preprocessor
    -Xcompiler=/Zc:__cplusplus"
)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler=-Zi -Xcompiler=-Od")
else()
    set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcompiler=-O2")
endif()
