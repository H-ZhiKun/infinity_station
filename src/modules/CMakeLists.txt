set(LIB_MODULES "lib_modules")
add_library(${LIB_MODULES} OBJECT)


message("===================== MODS BEGIN =====================")
foreach(MODS_DIR ${MODS_CONFIGS})
message("---------- Building ${MODS_DIR}  ----------")
    add_subdirectory(${MODS_DIR})
endforeach()
message("===================== MODS END   =====================")
