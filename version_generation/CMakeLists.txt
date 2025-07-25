
file(STRINGS "${CMAKE_SOURCE_DIR}/.version" CONFIG_LINES)

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
# 获取 Git 当前版本信息
execute_process(
    COMMAND git describe --tags --always --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 获取编译时间
execute_process(
  #
    COMMAND powershell -Command "Get-Date -Format 'yyyy-MM-dd HH:mm:ss'"
    OUTPUT_VARIABLE COMPILE_TIME
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 生成版本文件
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.js.in 
    ${CMAKE_SOURCE_DIR}/res/version.js
    @ONLY
)
