
file(GLOB_RECURSE SRC_SERS "*.cpp" "*.h")  # 递归遍历

add_library(lib_service OBJECT ${SRC_SERS})
target_sources(lib_service PUBLIC 
$<TARGET_OBJECTS:kit_module_base>
$<TARGET_OBJECTS:kit_utils>
$<TARGET_OBJECTS:kit_eventloop_monitor>
)      
target_link_libraries(lib_service PUBLIC shared_dependencies kit_module_base kit_utils kit_log kit_eventloop_monitor)