file(STRINGS "${CMAKE_SOURCE_DIR}/.version" CONFIG_LINES)
add_custom_target(copy_config_file ALL
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/.version"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
foreach(line IN LISTS CONFIG_LINES)
    string(FIND "${line}" "=" EQUAL_POS)
    if(NOT EQUAL_POS EQUAL -1)
        string(SUBSTRING "${line}" 0 ${EQUAL_POS} key)
        math(EXPR value_start "${EQUAL_POS} + 1")
        string(SUBSTRING "${line}" ${value_start} -1 value)
        string(STRIP "${key}" key)
        string(STRIP "${value}" value)
        set("${key}" "${value}")
    endif()
endforeach()

# ---- 板块子目录名称列表 BEGIN ----
set(MODS_CONFIGS "")
set(CTLS_CONFIGS "")
# ---- 板块子目录名称列表 END   ----


# ---- 匹配发布版本构建 ----
message("============================ CURRENT_VERSION <${CURRENT_VERSION}> ============================")
# 动态引入对应版本配置文件（如果存在）
set(VERSION_FILE "${CMAKE_SOURCE_DIR}/src/tis_version_cmake/${CURRENT_VERSION}.cmake")
if(EXISTS ${VERSION_FILE})
    include(${VERSION_FILE})
else()
    message(WARNING "No cmakelist file found for ${VERSION_FILE}")
endif()


add_subdirectory(kits)
add_subdirectory(modules)
add_subdirectory(controllers)
add_subdirectory(service)
