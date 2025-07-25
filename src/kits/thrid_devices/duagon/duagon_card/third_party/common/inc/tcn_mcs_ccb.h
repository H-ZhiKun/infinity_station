#if !defined (TCN_MCS_CCB_H)
#define TCN_MCS_CCB_H

/* ==========================================================================
 *
 *  Module      :   TCN_MCS_CCB.H
 *
 *  Purpose     :   Callback - i.e. Message Data Confirmation Procedures
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
/*                                                                            */
/*   Function Prototypes                                                      */
/*                                                                            */
/******************************************************************************/
#ifndef DG_MULTI_CARD

void        ccb_init (void);
void        ccb_clear_callback (void);
BOOLEAN1    ccb_call_confirm (UNSIGNED8 package_size);
BOOLEAN1    ccb_receive_confirm (UNSIGNED8 package_size);
void        ccb_reply_confirm (void);
UNSIGNED32  ccb_get_package_counter (void);

#endif

void        ccb_init_idx (DG_U8 card_index);

void        ccb_clear_callback_idx (DG_U8 card_index);

BOOLEAN1    ccb_call_confirm_idx (UNSIGNED8 package_size, DG_U8 card_index);

BOOLEAN1    ccb_receive_confirm_idx (UNSIGNED8 package_size, DG_U8 card_index);

void        ccb_reply_confirm_idx (DG_U8 card_index);

UNSIGNED32  ccb_get_package_counter_idx (DG_U8 card_index);

/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif


#endif /* !TCN_MCS_CCB_H */
