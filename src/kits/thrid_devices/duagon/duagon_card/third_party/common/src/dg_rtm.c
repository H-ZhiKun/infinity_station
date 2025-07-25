/* ==========================================================================
 *
 *  File      : DG_RTM.C
 *
 *  Purpose   : Duagon Real time management
 *
 *  Project   : Host Driver
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon AG, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */
#include "dg_rtm.h"
#include "os_def.h"
#include "dg_hdio.h"

#define RTM_HANDLE_INITIALIZED 255
#define RTM_NORMAL_HANDLE_THRESHOLD 200 /* if a handle is below this threshold, it is either valid or finished */
#define RTM_HANDLE_FNISHED 0

struct {
    EXT_HANDLE_P mvb_card_handle;
    DG_BOOL in_use;
} global_MVB_card=DG_INIT_GLOBAL_MVB_CARD;

DLL DG_DECL_PUBLIC DG_BOOL rtm_isWorking(RTM_HANDLE_P handle){
    return  (DG_BOOL)(handle && handle->handle < RTM_NORMAL_HANDLE_THRESHOLD && handle->handle != RTM_HANDLE_FNISHED);
}

DLL DG_DECL_PUBLIC void rtm_init_handle(RTM_HANDLE_P handle){
    handle->handle = RTM_HANDLE_INITIALIZED;
    handle->card_handle_defined = TRUE;
}

DLL DG_DECL_PUBLIC DG_BOOL rtm_isFinished(RTM_HANDLE_P handle) {
    return (DG_BOOL)(handle && handle->handle == RTM_HANDLE_FNISHED);
}

DLL DG_DECL_PUBLIC void rtm_setFinished(RTM_HANDLE_P handle){
    handle->handle = RTM_HANDLE_FNISHED;
}

DLL DG_DECL_PUBLIC DG_U8 rtm_get_handle(RTM_HANDLE_P handle) {
    return handle->handle;
}
DLL DG_DECL_PUBLIC void rtm_set_handle(RTM_HANDLE_P handle, DG_U8 command) {
    handle->handle = command;
}

DG_DECL_PUBLIC void card_init_handle(DG_U8 card_number, EXT_HANDLE_P handle) {
    if ( card_defined[card_number].defined ) {
        handle->card_index = card_defined[card_number].index;
        handle->card_handle_defined = TRUE;
    }
    handle->card_number = card_number;
}

DG_DECL_PUBLIC DG_U8 get_card_index(EXT_HANDLE_P ext_handle) {
    DG_U8 card_index=0;
    if ( ext_handle!=NULL ) {
        if (ext_handle->card_index) {
        card_index = ext_handle->card_index;
        }
    }
    return card_index;
}

