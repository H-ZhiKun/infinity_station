set(KIT_NAME "kit_log")
file(GLOB SRC_CURRENT ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
find_package(spdlog CONFIG REQUIRED)

add_library(${KIT_NAME} SHARED ${SRC_CURRENT})

target_compile_definitions(${KIT_NAME} PRIVATE LOGGERCORE_EXPORTS)

target_link_libraries(${KIT_NAME} PUBLIC
    shared_dependencies
    spdlog::spdlog
)
