#if !defined (TCN_MCS_CGF_H)
#define TCN_MCS_CGF_H

/* ==========================================================================
 *
 *  Module      :   TCN_MCS_CGF.H
 *
 *  Purpose     :   General Function Interface
 *
 *  Project     :   MVB Client-Server Model (MVB Client Driver Software)
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon AG, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * --------------------------------------------------------------------------
 *
 *
 * ==========================================================================
 */


/******************************************************************************/
/*                                                                            */
/*   Include Files                                                            */
/*                                                                            */
/******************************************************************************/
#include "tcn_mcs_def.h"

/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (begin)                                            */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif

/******************************************************************************/
/*   Do not use multithread   **DO NOT CHANGE THIS**                          */
/******************************************************************************/
#define O_OS_SINGLE
#ifdef TT_OSL_LIB_POSIX
#undef TT_OSL_LIB_POSIX
#define TT_OSL_LIB_POSIX 0
#endif

#ifndef FunctionDef
#   define FunctionDef
#endif


/******************************************************************************/
/*                                                                            */
/*   General Definitions                                                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*   Server Status                                                            */
/******************************************************************************/
#define CGF_SERVER_STATUS_STOP            0
#define CGF_SERVER_STATUS_RUN             1
#define CGF_SERVER_STATUS_ERROR           2
#define CGF_SERVER_STATUS_ERROR_DYNAMIC   3

/******************************************************************************/
/*   Error Codes                                                              */
/******************************************************************************/
#define GF_OK                       0
#define GF_ERROR                    2
#define GF_CONFIG                   3
#define GF_MEMORY                   4
#define GF_UNKNOWN_TS               5
#define GF_RANGE                    6

#define GF_DEVICE_ADDRESS           8
#define GF_LINE                     9
#define GF_TREPLY                   10

/******************************************************************************/
/*   Device Errors                                                            */
/******************************************************************************/
#define GF_DEVICE_ERROR_OK                0
#define GF_DEVICE_ERROR_SERVER_MISSING    1
#define GF_DEVICE_ERROR_SERVER_FAILED     2
#define GF_DEVICE_ERROR_RETURN_INSTANCE   10
#define GF_DEVICE_ERROR_HW                50  /* ERROR HW  > 50  */
#define GF_DEVICE_ERROR_PIL               100 /* ERROR PIL > 100 */

/******************************************************************************/
/*   Device Status                                                            */
/******************************************************************************/
#define GF_DEVICE_STATUS_OK                        0x0000

#define GF_DEVICE_STATUS_CLIENT_LIFESIGN_TIMEOUT   0x0001
#define GF_DEVICE_STATUS_SERVER_RECEIVE_CHANNEL    0x0002
#define GF_DEVICE_STATUS_SERVER_RECEIVE_CRC        0x0004
#define GF_DEVICE_STATUS_MVBD_OVERTEMPERATURE      0x0010

#define GF_DEVICE_STATUS_SERVER_MISSING            0x0100
#define GF_DEVICE_STATUS_SERVER_FAILED             0x0200
#define GF_DEVICE_STATUS_CLIENT_RECEIVE_CHANNEL    0x0400
#define GF_DEVICE_STATUS_CLIENT_RECEIVE_CRC        0x0800
#define GF_DEVICE_STATUS_CLIENT_RECEIVE_TIMEOUT    0x1000
#define GF_DEVICE_STATUS_CLIENT_SEND_CHANNEL       0x2000
#define GF_DEVICE_STATUS_CLIENT_SEND_TIMEOUT       0x4000


/******************************************************************************/
/*                                                                            */
/*   Global Variables                                                         */
/*                                                                            */
/******************************************************************************/

extern UNSIGNED8  cgf_server_status;

extern BOOLEAN1   bDone_gf_open_device;
extern BOOLEAN1   bDone_gf_init_device;
extern BOOLEAN1   bDone_am_init;
extern BOOLEAN1   bDone_am_announce_device;

/******************************************************************************/
/*                                                                            */
/*   Type Definitions                                                         */
/*                                                                            */
/******************************************************************************/
typedef UNSIGNED16   GF_RESULT;

typedef UNSIGNED16   GF_DEVICE_ERROR;

typedef UNSIGNED16   GF_DEVICE_STATUS;

typedef void (*GF_DEVICE_STATUS_CALLBACK)
(
   GF_DEVICE_STATUS  device_status
);


/******************************************************************************/
/*                                                                            */
/*   Function Prototypes                                                      */
/*                                                                            */
/******************************************************************************/
void cgf_supervisor (DG_U8 card_index);

void           cgf_put_device_status (UNSIGNED16 device_status, DG_U8 card_index);

GF_RESULT      cgf_poll_server_callback (UNSIGNED32 max_package_number, BOOLEAN1 *bNoCallbacks, BOOLEAN1 *bCallbackBusy, DG_U8 card_index);

#if defined (O_INIT_GLOBAL_DATA)
void FunctionDef gf_init_global_data (void);
#endif

#ifndef DG_MULTI_CARD

GF_RESULT FunctionDef 
gf_open_device 
(
    void *             p_device_config,
    UNSIGNED32         client_life_sign_timeout,
    GF_DEVICE_ERROR *  device_error
 );
GF_RESULT FunctionDef 
    gf_init_device (void);
GF_RESULT FunctionDef 
    gf_get_device_status (GF_DEVICE_STATUS * device_status);
GF_RESULT FunctionDef
   gf_set_device_status_callback (GF_DEVICE_STATUS_CALLBACK device_status_callback);

#if defined (O_OS_SINGLE)
/******************************************************************************/
/*   Only for single task operation system:                                   */
/******************************************************************************/
GF_RESULT FunctionDef 
    gf_poll_callback (UNSIGNED32 max_package_number, BOOLEAN1 *bCallbackBusy);
#endif

#endif
/****************************************************************************/
/****************************************************************************/
GF_RESULT FunctionDef gf_open_device_idx
(
    void *             p_device_config,
    UNSIGNED32         client_life_sign_timeout,
    GF_DEVICE_ERROR *  device_error,
    DG_U8 card_index
);
GF_RESULT FunctionDef
    gf_init_device_idx (DG_U8 card_index);
GF_RESULT FunctionDef
   gf_get_device_status_idx (GF_DEVICE_STATUS * device_status, DG_U8 card_index);
GF_RESULT FunctionDef
    gf_set_device_status_callback_idx (GF_DEVICE_STATUS_CALLBACK device_status_callback, DG_U8 card_index);

#if defined (O_OS_SINGLE)
/******************************************************************************/
/*   Only for single task operation system:                                   */
/******************************************************************************/
GF_RESULT FunctionDef
    gf_poll_callback_idx(UNSIGNED32 max_package_number, BOOLEAN1 *bCallbackBusy,DG_U8 card_index);
#endif

/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif


#endif /* !TCN_MCS_CGF_H */
