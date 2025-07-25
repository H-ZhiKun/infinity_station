#if !defined (TCN_MCS_DEF_H)
#define TCN_MCS_DEF_H

/* ==========================================================================
 *
 *  Module      :   TCN_MCS_DEF.H
 *
 *  Purpose     :   General Definitions
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
/*   C linkage declaration (begin)                                            */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif


/******************************************************************************/
/*                                                                            */
/*   General Definitions                                                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*   Channel Numbers Client (0-3)                                             */
/******************************************************************************/
#define MCS_CLIENT_CHANNEL_NUMBER_MAX           ((UNSIGNED8) 4)

#define MCS_CLIENT_CHANNEL_NUMBER_LIFESIGN      ((UNSIGNED8) 0)
#define MCS_CLIENT_CHANNEL_NUMBER_CONFIG        ((UNSIGNED8) 1)
#define MCS_CLIENT_CHANNEL_NUMBER_PROCESS_DATA  ((UNSIGNED8) 2)
#define MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA  ((UNSIGNED8) 3)

/******************************************************************************/
/*   Channel Numbers Server (0-3)                                             */
/******************************************************************************/
#define MCS_SERVER_CHANNEL_NUMBER_MAX           ((UNSIGNED8) 3)

#define MCS_SERVER_CHANNEL_NUMBER_LIFESIGN      ((UNSIGNED8) 0)
#define MCS_SERVER_CHANNEL_NUMBER_RETURN        ((UNSIGNED8) 1)
#define MCS_SERVER_CHANNEL_NUMBER_CALLBACK      ((UNSIGNED8) 2)

/******************************************************************************/
/*   Function Numbers                                                         */
/******************************************************************************/

/* General */
#define MCS_RETURN_NUMBER_GF_UNKNOWN_FUNCTION            ((UNSIGNED8) 1)

/* LifeSign */
#define MCS_FUNCTION_NUMBER_GF_LIFE_SIGN                 ((UNSIGNED8) 2)
#define MCS_RETURN_NUMBER_GF_LIFE_SIGN                   ((UNSIGNED8) 3)

/* Configuration */
#define MCS_FUNCTION_NUMBER_GF_RESTART_DEVICE            ((UNSIGNED8) 10)
#define MCS_RETURN_NUMBER_GF_RESTART_DEVICE              ((UNSIGNED8) 11)
#define MCS_FUNCTION_NUMBER_GF_INIT_DEVICE               ((UNSIGNED8) 12)
#define MCS_RETURN_NUMBER_GF_INIT_DEVICE                 ((UNSIGNED8) 13)
#define MCS_FUNCTION_NUMBER_GF_CLOSE_DEVICE              ((UNSIGNED8) 14)
#define MCS_RETURN_NUMBER_GF_CLOSE_DEVICE                ((UNSIGNED8) 15)
#define MCS_FUNCTION_NUMBER_GF_GET_HARDWARE_INFO         ((UNSIGNED8) 16)
#define MCS_RETURN_NUMBER_GF_GET_HARDWARE_INFO           ((UNSIGNED8) 17)
#define MCS_FUNCTION_NUMBER_GF_SERVICE_COMMAND           ((UNSIGNED8) 18)
#define MCS_RETURN_NUMBER_GF_SERVICE_COMMAND             ((UNSIGNED8) 19)
#define MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_INFO         ((UNSIGNED8) 20)
#define MCS_RETURN_NUMBER_GF_GET_CALLBACK_INFO           ((UNSIGNED8) 21)
#define MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_NEXT         ((UNSIGNED8) 22)
#define MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_PACKAGES     ((UNSIGNED8) 24)
#define MCS_FUNCTION_NUMBER_GF_GET_DEVICE_MEMORY         ((UNSIGNED8) 26)
#define MCS_RETURN_NUMBER_GF_GET_DEVICE_MEMORY           ((UNSIGNED8) 27)
#define MCS_FUNCTION_NUMBER_GF_GET_SOFTWARE_IDENT_NR     ((UNSIGNED8) 28)
#define MCS_RETURN_NUMBER_GF_GET_SOFTWARE_IDENT_NR       ((UNSIGNED8) 29)

#define MCS_FUNCTION_NUMBER_GF_MVB_INIT                  ((UNSIGNED8) 30)
#define MCS_RETURN_NUMBER_GF_MVB_INIT                    ((UNSIGNED8) 31)
#define MCS_FUNCTION_NUMBER_GF_MVB_START                 ((UNSIGNED8) 32)
#define MCS_RETURN_NUMBER_GF_MVB_START                   ((UNSIGNED8) 33)
#define MCS_FUNCTION_NUMBER_GF_MVB_STOP                  ((UNSIGNED8) 34)
#define MCS_RETURN_NUMBER_GF_MVB_STOP                    ((UNSIGNED8) 35)

/* Process Data Ports */
#define MCS_FUNCTION_NUMBER_LP_INIT                      ((UNSIGNED8) 100)
#define MCS_RETURN_NUMBER_LP_INIT                        ((UNSIGNED8) 101)
#define MCS_FUNCTION_NUMBER_LP_PUT_DATASET               ((UNSIGNED8) 102)
#define MCS_RETURN_NUMBER_LP_PUT_DATASET                 ((UNSIGNED8) 103)
#define MCS_FUNCTION_NUMBER_LP_GET_DATASET               ((UNSIGNED8) 104)
#define MCS_RETURN_NUMBER_LP_GET_DATASET                 ((UNSIGNED8) 105)
#define MCS_FUNCTION_NUMBER_LP_CMD_DATASET               ((UNSIGNED8) 106)
#define MCS_RETURN_NUMBER_LP_CMD_DATASET                 ((UNSIGNED8) 107)

/* Message Data */
#define MCS_FUNCTION_NUMBER_AM_INIT                      ((UNSIGNED8) 120)
#define MCS_RETURN_NUMBER_AM_INIT                        ((UNSIGNED8) 121)
#define MCS_FUNCTION_NUMBER_AM_ANNOUNCE_DEVICE           ((UNSIGNED8) 122)
#define MCS_RETURN_NUMBER_AM_ANNOUNCE_DEVICE             ((UNSIGNED8) 123)
#define MCS_FUNCTION_NUMBER_AM_SHOW_BUSSES               ((UNSIGNED8) 124)
#define MCS_RETURN_NUMBER_AM_SHOW_BUSSES                 ((UNSIGNED8) 125)
#define MCS_FUNCTION_NUMBER_AM_SET_CURRENT_TC            ((UNSIGNED8) 126)
#define MCS_RETURN_NUMBER_AM_SET_CURRENT_TC              ((UNSIGNED8) 127)


#define MCS_FUNCTION_NUMBER_AM_CALL_REQUEST              ((UNSIGNED8) 130)
#define MCS_RETURN_NUMBER_AM_CALL_REQUEST                ((UNSIGNED8) 131)
#define MCS_RETURN_NUMBER_AM_CALL_CONFIRM                ((UNSIGNED8) 133)

#define MCS_FUNCTION_NUMBER_AM_CALL_CANCEL               ((UNSIGNED8) 134)
#define MCS_RETURN_NUMBER_AM_CALL_CANCEL                 ((UNSIGNED8) 135)

#define MCS_FUNCTION_NUMBER_AM_BIND_REPLIER              ((UNSIGNED8) 140)
#define MCS_RETURN_NUMBER_AM_BIND_REPLIER                ((UNSIGNED8) 141)

#define MCS_FUNCTION_NUMBER_AM_UNBIND_REPLIER            ((UNSIGNED8) 142)
#define MCS_RETURN_NUMBER_AM_UNBIND_REPLIER              ((UNSIGNED8) 143)

#define MCS_FUNCTION_NUMBER_AM_RECEIVE_REQUEST           ((UNSIGNED8) 144)
#define MCS_RETURN_NUMBER_AM_RECEIVE_REQUEST             ((UNSIGNED8) 145)

#define MCS_FUNCTION_NUMBER_AM_RECEIVE_CANCEL            ((UNSIGNED8) 146)
#define MCS_RETURN_NUMBER_AM_RECEIVE_CANCEL              ((UNSIGNED8) 147)
#define MCS_RETURN_NUMBER_AM_RECEIVE_CONFIRM             ((UNSIGNED8) 149)

#define MCS_FUNCTION_NUMBER_AM_REPLY_REQUEST             ((UNSIGNED8) 150)
#define MCS_RETURN_NUMBER_AM_REPLY_REQUEST               ((UNSIGNED8) 151)
#define MCS_RETURN_NUMBER_AM_REPLY_CONFIRM               ((UNSIGNED8) 153)

/* Bus Administrator Functionality */
#define MCS_FUNCTION_NUMBER_BA_SETUP_BUS_ADMINISTRATOR   ((UNSIGNED8) 170)
#define MCS_RETURN_NUMBER_BA_SETUP_BUS_ADMINISTRATOR     ((UNSIGNED8) 171)
#define MCS_FUNCTION_NUMBER_BA_SEND_MF                   ((UNSIGNED8) 172)
#define MCS_RETURN_NUMBER_BA_SEND_MF                     ((UNSIGNED8) 173)

/* Device Status Words */
#define MCS_FUNCTION_NUMBER_DSW_INIT                     ((UNSIGNED8) 180)
#define MCS_RETURN_NUMBER_DSW_INIT                       ((UNSIGNED8) 181)
#define MCS_FUNCTION_NUMBER_DSW_READ                     ((UNSIGNED8) 182)
#define MCS_RETURN_NUMBER_DSW_READ                       ((UNSIGNED8) 183)

/******************************************************************************/
/*   Data Types                                                               */
/******************************************************************************/
#define MCS_DATA_TYPE_1    ((UNSIGNED16) 1)
#define MCS_DATA_TYPE_2    ((UNSIGNED16) 2)
#define MCS_DATA_TYPE_4    ((UNSIGNED16) 4)

/******************************************************************************/
/*   Logical Channels (Client)                                                */
/*   - package header                                                         */
/*     0) dummy byte (necessary for word allignment of package data)          */
/*     1) start delimiter                                                     */
/*     2) channel number                                                      */
/*     3) package size                                                        */
/*     4) package CRC                                                         */
/*   - package data                                                           */
/*     1) function number                                                     */
/*     2) instance number                                                     */
/*     3) data bytes (e.g. parameter, data, ...)                              */
/******************************************************************************/
#define MCS_CLIENT_CHANNEL_HEADER_START_DELIMITER  ((UNSIGNED8) 0x55)

#define MCS_CLIENT_CHANNEL_DUMMY_SIZE              ((UNSIGNED8) 0)
#define MCS_CLIENT_CHANNEL_HEADER_SIZE             ((UNSIGNED8) 4)
#define MCS_CLIENT_CHANNEL_PACKAGE_SIZE_MAX        ((UNSIGNED8) 70)

/* send_package_size = dummy_size + header_size + package_size */
#define MCS_CLIENT_CHANNEL_SEND_PACKAGE_SIZE       ((UNSIGNED8) 74)

/******************************************************************************/
/*   Logical Channels (Server)                                                */
/*   - package header                                                         */
/*     0) dummy byte (necessary for word allignment of package data)          */
/*     1) start delimiter                                                     */
/*     2) channel number                                                      */
/*     3) package size                                                        */
/*     4) package CRC                                                         */
/*     5) device status                                                       */
/*   - package data                                                           */
/*     1) return number                                                       */
/*     2) instance number                                                     */
/*     3) data bytes (e.g. parameter, data, ...)                              */
/******************************************************************************/
#define MCS_SERVER_CHANNEL_HEADER_START_DELIMITER  ((UNSIGNED8) 0x55)

#define MCS_SERVER_CHANNEL_DUMMY_SIZE              ((UNSIGNED8) 1)
#define MCS_SERVER_CHANNEL_HEADER_SIZE             ((UNSIGNED8) 5)
#define MCS_SERVER_CHANNEL_PACKAGE_SIZE_MAX        ((UNSIGNED8) 40)

/* send_package_size = dummy_size + header_size + package_size_max */
#define MCS_SERVER_CHANNEL_SEND_PACKAGE_SIZE       ((UNSIGNED8) 46)


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif


#endif /* !TCN_MCS_DEF_H */
