/* ==========================================================================
 *
 *  File      : DG_RTM.H
 *
 *  Purpose   : Duagon Real time management
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

#ifndef DG_RTM_H
#define DG_RTM_H

#include "os_def.h"
#include "dg_rpc_g.h"

#define RTM_MAX_NO_HANDLES 32

typedef struct {
    DG_U8 handle;
    DG_BOOL rt_handle_defined;
    DG_U8 card_index;
    DG_BOOL card_handle_defined;
    DG_U8 card_number;
} DG_EXT_HANDLE;

#define INIT_EXT_HANDLE {0,0,0,0,0}

typedef DG_EXT_HANDLE DG_RTM_HANDLE;
typedef DG_EXT_HANDLE RTM_HANDLE;
typedef RTM_HANDLE *RTM_HANDLE_P;
typedef DG_EXT_HANDLE *EXT_HANDLE_P;

DG_DECL_PUBLIC DG_BOOL rtm_isWorking(RTM_HANDLE_P handle);
DG_DECL_PUBLIC DG_BOOL rtm_isFinished(RTM_HANDLE_P handle);
DG_DECL_PUBLIC void rtm_init_handle(RTM_HANDLE_P handle);
DG_DECL_PUBLIC void card_init_handle(DG_U8 card_number, EXT_HANDLE_P handle);


/* the following functions are for driver use only. never use them outside hdc */
DG_DECL_PUBLIC void rtm_setFinished(RTM_HANDLE_P handle);
DG_DECL_PUBLIC DG_U8 rtm_get_handle(RTM_HANDLE_P handle);
DG_DECL_PUBLIC void rtm_set_handle(RTM_HANDLE_P handle, DG_U8 command);
DG_DECL_PUBLIC DG_U8 get_card_index(EXT_HANDLE_P ext_handle);

#define DG_INIT_GLOBAL_MVB_CARD {NULL,0}

#endif /* DG_RTM_H */
