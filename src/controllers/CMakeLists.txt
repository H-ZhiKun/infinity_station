set(LIB_CONTROLLERS "lib_controllers")

add_library(${LIB_CONTROLLERS} OBJECT)   

message("===================== CTLS BEGIN =====================")
foreach(CTLS_DIR ${CTLS_CONFIGS})
    add_subdirectory(${CTLS_DIR})
endforeach()
message("===================== CTLS END   =====================")
