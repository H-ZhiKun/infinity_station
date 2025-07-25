cmake_minimum_required(VERSION 3.21)
project(togee_infinity_station VERSION 2.1 LANGUAGES CXX)
# 全局预配置项
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 标准化架构名称
if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    set(TARGET_ARCH "x64")
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    set(TARGET_ARCH "arm64")
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()
# 检查当前平台
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(TARGET_OS "windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TARGET_OS "linux")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin") 
    set(TARGET_OS "macos")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# 设置第三方库根目录
set(THIRD_PARTY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/${VCPKG_TARGET_TRIPLET}")
set(THIRD_PARTY_BIN "")
set(THIRD_PARTY_INCLUDE "${THIRD_PARTY_ROOT}/include")
set(THIRD_PARTY_LIB "")
# 动态设置输出目录
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
if(BUILD_TYPE_LOWER STREQUAL "debug")
    set(THIRD_PARTY_BIN "${THIRD_PARTY_ROOT}/debug/bin")
    set(THIRD_PARTY_LIB "${THIRD_PARTY_ROOT}/debug/lib")
    add_compile_definitions(QT_QML_DEBUG=1)
else()
    set(THIRD_PARTY_BIN "${THIRD_PARTY_ROOT}/bin")
    set(THIRD_PARTY_LIB "${THIRD_PARTY_ROOT}/lib")
    add_compile_definitions(QT_QML_DEBUG=0)
endif()

# 添加选项以启用或禁用 AddressSanitizer
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
message(STATUS "AddressSanitizer ${CMAKE_CXX_COMPILER_ID}")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.8))
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/utf-8 /Wall)
    else()
        message(WARNING "AddressSanitizer is not supported by the current compiler")
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-DNOMINMAX)
    add_compile_options(/utf-8 /Zc:preprocessor)
endif()

# 添加子文件夹路径到 CMAKE_PREFIX_PATH
set(CMAKE_PREFIX_PATH "${THIRD_PARTY_ROOT}/share" ${CMAKE_PREFIX_PATH})
# 设置库输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs) # 静态库 .lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs) # 动态库 .dll/.so
# 设置运行目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${VCPKG_TARGET_TRIPLET}-${BUILD_TYPE_LOWER}")
message("=================== Platform Begin ===================")
message("Build Type: ${BUILD_TYPE_LOWER}")
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif()
message("Target Platform: ${VCPKG_TARGET_TRIPLET}")
message("current compiler: ${CMAKE_CXX_COMPILER_ID}")
message("Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("=================== Platform End   ===================")


# 添加基础源文件
file(GLOB SRC_GLOBAL ${CMAKE_CURRENT_SOURCE_DIR}/src/tis_global/*.h)
# 统一依赖管理 begin
find_package(Qt6 6.8 REQUIRED COMPONENTS Core Quick Gui Qml QuickControls2 Network Sql Charts )
find_package(jsoncpp CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(libzippp CONFIG REQUIRED)
# find_package(vld CONFIG)
# find_package(OpenSSL REQUIRED)
add_library(shared_dependencies INTERFACE ${SRC_GLOBAL})
target_include_directories(shared_dependencies INTERFACE ${THIRD_PARTY_INCLUDE} ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(shared_dependencies INTERFACE
    Qt6::Core Qt6::Quick Qt6::GuiPrivate Qt6::Qml Qt6::QuickControls2 Qt6::Network Qt6::Sql Qt6::Charts 
    JsonCpp::JsonCpp
    yaml-cpp::yaml-cpp
    libzippp::libzippp
)

if(WIN32)
add_custom_target(copy_crt ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${THIRD_PARTY_BIN}/crt ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
endif()
# 判断 MSVC 且非 Debug 模式
if(MSVC AND NOT BUILD_TYPE_LOWER STREQUAL "debug")
    add_custom_target(copy_daemon ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/tools/tis_daemon.exe" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/tools/tis_daemon_start.bat" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )
endif()

# 统一依赖管理 end

qt_standard_project_setup(REQUIRES 6.8)
# 查找 tis_compiler
find_program(TIS_COMPILER_PATH
    NAMES tis_compiler tis_compiler.exe  # 支持跨平台名称
    PATHS ${CMAKE_SOURCE_DIR}/tools   # 自定义搜索路径
          /usr/local/bin  # 其他可能的路径
    NO_DEFAULT_PATH  # 仅在指定路径中查找
)

# 如果未找到 tis_generator，报错
if (NOT TIS_COMPILER_PATH)
    message(FATAL_ERROR "tis_generator not found! Please build tis_generator first.")
else()
    message(STATUS "Found tis_generator: ${TIS_COMPILER_PATH}")
    set(TIS_RES_CMAKE "${CMAKE_SOURCE_DIR}/src/tis_res_cmake")
    if(NOT EXISTS "${TIS_RES_CMAKE}")
        message(STATUS "Creating directory: ${TIS_RES_CMAKE}")
        file(MAKE_DIRECTORY "${TIS_RES_CMAKE}")
    endif()
    # 1. 先调用程序生成文件
    execute_process(
        COMMAND ${TIS_COMPILER_PATH} ${CMAKE_CURRENT_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE gen_result
        OUTPUT_VARIABLE gen_output
        ERROR_VARIABLE gen_error
    )

    # 检查生成结果是否成功
    if (gen_result EQUAL 0)
        message(STATUS "Files generated successfully.")
    else()
        message(FATAL_ERROR "Failed to generate files: ${gen_error}")
    endif()

endif()


qt_add_executable(${PROJECT_NAME}
    src/main.cpp
)

# --- 动态生成清单文件并嵌入（仅 Windows） ---
if (MSVC AND NOT BUILD_TYPE_LOWER STREQUAL "debug")
    # 定义生成的清单文件路径
    set(GENERATED_MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/generated.manifest")
    # 动态生成清单内容
    file(WRITE ${GENERATED_MANIFEST}
        "<?xml version='1.0' encoding='UTF-8' standalone='yes'?>\n"
        "<assembly xmlns='urn:schemas-microsoft-com:asm.v1' manifestVersion='1.0'>\n"
        "  <trustInfo xmlns='urn:schemas-microsoft-com:asm.v3'>\n"
        "    <security>\n"
        "      <requestedPrivileges>\n"
        "        <requestedExecutionLevel level='requireAdministrator' uiAccess='false'/>\n"
        "      </requestedPrivileges>\n"
        "    </security>\n"
        "  </trustInfo>\n"
        "</assembly>\n"
    )
    # 将生成的清单文件作为资源添加到目标
    target_sources(${PROJECT_NAME} PRIVATE ${GENERATED_MANIFEST})
    # 嵌入清单到可执行文件
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/MANIFEST:EMBED"
    )

    # 设置 Windows 目标版本（Vista+）
    add_compile_definitions(_WIN32_WINNT=0x0600)
    # 强制指定 UAC 执行级别
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\""
    )

endif()


set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)


add_subdirectory(src)
add_subdirectory(version_generation)

file(GLOB_RECURSE VERSION_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/ ${CMAKE_CURRENT_SOURCE_DIR}/res/version.js)
include(${TIS_RES_CMAKE}/tis_res.cmake)


qt_add_qml_module(${PROJECT_NAME}
    URI infinity_station
    VERSION 1.0
    QML_FILES 
        ${GLOBAL_RES_QML}
    RESOURCES
        ${GLOBAL_RES_FILES}
        ${VERSION_FILES}
        res/qml/public_controls/qmldir   # 这里单独加上 qmldir
    RESOURCE_PREFIX "/"
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    lib_controllers 
    lib_modules 
    lib_service 
)


#添加部署脚本
#为了确保执行正确，所以使用add_custom_target
#在确认生成正确的release工程后
# 终端使用命令启用脚本：
# cmake --build build --target Script-DeployRelease
if (BUILD_TYPE_LOWER MATCHES "release")
    find_program(WIN_DEPLOY_QT6 NAMES windeployqt6)
    message("========== DEPLOY BEGIN ==========")
    message("${WIN_DEPLOY_QT6}")
    message("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    message("========== DEPLOY END   ==========")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/dist
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/dist
    )
    add_custom_target(Script-DeployRelease
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${CMAKE_SOURCE_DIR}/dist
            COMMAND ${WIN_DEPLOY_QT6} --qmldir ${CMAKE_SOURCE_DIR}/res/qml --plugindir ${CMAKE_SOURCE_DIR}/dist/plugins --compiler-runtime ${CMAKE_SOURCE_DIR}/dist/${PROJECT_NAME}.exe
            COMMENT "Windows Deploying Qt Dependencies After Build."
            SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif ()


#  * windeployqt6 常用参数总结
#  * 
#  * 【基本选项】
#  * --release                 打包 Release 版本程序 (默认)
#  * --debug                   打包 Debug 版本程序
#  * --force                   强制重新复制所有文件（默认已覆盖，通常无需）
#  * --dir <目录>              指定打包到的目标目录（不写时默认放到exe所在目录）
#  * 
#  * 【运行库相关】
#  * --compiler-runtime        同时打包 Visual C++ 运行库（CRT，通常用于 Release版）
#  * --no-compiler-runtime     不打包 VC++ 运行库（如果自行安装 vc_redist 可用此选项）
#  * 
#  * 【模块和插件相关】
#  * --qmlmodule <目录>         指定 QML 模块目录 (旧版为 --qmldir，Qt6推荐--qmlmodule)
#  * --no-plugins              不打包 Qt 插件 (如 platforms, imageformats 等)
#  * --no-translations         不打包翻译文件 (.qm)，减少体积
#  * --no-opengl-sw            不打包 Qt OpenGL software renderer (opengl32sw.dll)
#  * --no-quick-import         不自动分析 QtQuick 相关 QML模块依赖（减少分析）
#  * 
#  * 【高级选项】
#  * --dry-run                 模拟执行，显示将复制哪些文件，但不实际复制
#  * --json                    以 JSON 格式输出分析结果（配合脚本使用）
#  * --verbose                 输出详细日志
#  * --list                    只列出将会复制的文件，不实际复制
#  * --pdb                     同时复制 PDB 调试符号文件（通常用于 Debug调试）
#  * 
#  * 【注意事项】
#  * - 目标目录存在同名文件时，windeployqt6 会直接覆盖，无提示。
#  * - 若要确保干净打包，建议打包前手动清空目标目录。
#  * - --compiler-runtime 打包的是 Release版 VC++ runtime。
#  *   如果需要 Debug版 CRT（带d的DLL，比如 ucrtbased.dll），需要自己单独处理。
