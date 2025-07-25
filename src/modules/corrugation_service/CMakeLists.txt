
message("---------- Building Corrugationbaselib  ----------")
add_subdirectory(Corrugationbaselib )
file(GLOB_RECURSE CURRENT_Corrugation_SRC ${CMAKE_CURRENT_SOURCE_DIR}/CorrugationService.cpp 
                                 ${CMAKE_CURRENT_SOURCE_DIR}/CorrugationService.h)

set(SRC_CORRUGATION_FROM_SUB "${CURRENT_MVB_SRC}" PARENT_SCOPE)
set(MVB_LIB "mvbbaselib" PARENT_SCOPE)
