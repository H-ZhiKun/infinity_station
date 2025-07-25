#if !defined (TCN_MCS_CLCH_H)
#define TCN_MCS_CLCH_H

/* ==========================================================================
 *
 *  Module      :   TCN_MCS_CLCH.H
 *
 *  Purpose     :   Logical Channel Communication
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
#include "tcn_def.h"


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (begin)                                            */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif

#if (0 == DG_HOST_SWAP)    /* defined in the os_def.h; '1' if big endian host */
#   define O_LE
#endif

/******************************************************************************/
/*                                                                            */
/*   General Definitions                                                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*   Error Codes                                                              */
/******************************************************************************/
#define CLCH_OK                  0
#define CLCH_ERROR               1
#define CLCH_NOT_ENOUGH_MEMORY   2
#define CLCH_ALLOCATE_MEMORY     3

/******************************************************************************/
/*   Default Instance Number                                                  */
/******************************************************************************/
#define CLCH_INSTANCE_NUMBER_DEFAULT 0


/******************************************************************************/
/*                                                                            */
/*   Global Variables                                                         */
/*                                                                            */
/******************************************************************************/
extern UNSIGNED8 clch_return_number;


/******************************************************************************/
/*                                                                            */
/*   Type Definitions                                                         */
/*                                                                            */
/******************************************************************************/
typedef UNSIGNED16   CLCH_RESULT;


/******************************************************************************/
/*                                                                            */
/*   Function Prototypes                                                      */
/*                                                                            */
/******************************************************************************/
void        clch_channel_close (UNSIGNED16 channel_number);

#ifndef DG_MULTI_CARD

void        clch_init (void);
void        clch_channel_open (UNSIGNED16 channel_number);
void        clch_channel_send_byte (UNSIGNED16 channel_number, UNSIGNED8 data_byte);
void        clch_channel_send_word (UNSIGNED16 channel_number, const UNSIGNED16 * p_data);
void        clch_channel_send_long (UNSIGNED16 channel_number, const UNSIGNED32 * p_data);
CLCH_RESULT clch_channel_send_array_byte (UNSIGNED16 channel_number, const UNSIGNED8 * p_data, UNSIGNED32 number_data);
void        clch_channel_send_array_word (UNSIGNED16 channel_number, const UNSIGNED16 * p_data, UNSIGNED16 number_data);
CLCH_RESULT clch_channel_send_package (UNSIGNED16 channel_number, BOOLEAN1 bFirstPackage, BOOLEAN1 bLastPackage);
UNSIGNED8   clch_channel_receive (UNSIGNED8 *p_return_number, UNSIGNED8 *p_package_size);
CLCH_RESULT clch_wait_for_receive (UNSIGNED8 return_number);
CLCH_RESULT clch_wait_for_receive_dynamic (UNSIGNED8 * package_size);
void        clch_receive_byte (UNSIGNED8 * p_data);
void        clch_receive_word (UNSIGNED16 * p_data);
void        clch_receive_long (UNSIGNED32 * p_data);
void        clch_receive_array_byte (UNSIGNED8 * p_data, UNSIGNED32 number_data);
void        clch_receive_array_word (UNSIGNED16 * p_data, UNSIGNED32 number_data);
CLCH_RESULT clch_receive_check_crc (void);

#endif

void        clch_init_idx (DG_U8 card_index);
void        clch_channel_open_idx (UNSIGNED16 channel_number, DG_U8 card_index);
void        clch_channel_send_byte_idx (UNSIGNED16 channel_number, UNSIGNED8 data_byte, DG_U8 card_index);
void        clch_channel_send_word_idx (UNSIGNED16 channel_number, const UNSIGNED16 * p_data, DG_U8 card_index);
void        clch_channel_send_long_idx(UNSIGNED16 channel_number, const UNSIGNED32 * p_data, DG_U8 card_index);
CLCH_RESULT clch_channel_send_array_byte_idx (UNSIGNED16 channel_number, const UNSIGNED8 * p_data, UNSIGNED32 number_data, DG_U8 card_index);
void        clch_channel_send_array_word_idx (UNSIGNED16 channel_number, const UNSIGNED16 * p_data, UNSIGNED16 number_data, DG_U8 card_index);
CLCH_RESULT clch_channel_send_package_idx (UNSIGNED16 channel_number, BOOLEAN1 bFirstPackage, BOOLEAN1 bLastPackage, DG_U8 card_index);
UNSIGNED8   clch_channel_receive_idx (UNSIGNED8 *p_return_number, UNSIGNED8 *p_package_size, DG_U8 card_index);
CLCH_RESULT clch_wait_for_receive_idx (UNSIGNED8 return_number, DG_U8 card_index);
CLCH_RESULT clch_wait_for_receive_dynamic_idx (UNSIGNED8 * package_size, DG_U8 card_index);
void        clch_receive_byte_idx (UNSIGNED8 * p_data, DG_U8 card_index);
void        clch_receive_word_idx (UNSIGNED16 * p_data, DG_U8 card_index);
void        clch_receive_long_idx (UNSIGNED32 * p_data, DG_U8 card_index);
void        clch_receive_array_byte_idx (UNSIGNED8 * p_data, UNSIGNED32 number_data, DG_U8 card_index);
void        clch_receive_array_word_idx (UNSIGNED16 * p_data, UNSIGNED32 number_data, DG_U8 card_index);
CLCH_RESULT clch_receive_check_crc_idx (DG_U8 card_index);


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif


#endif /* !TCN_MCS_CLCH_H */
