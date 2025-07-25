/* ==========================================================================
 *
 *  WARNING: DO NOT EDIT THIS FILE. YOUR CHANGES WILL BE LOST WHEN THE FILE 
 *  IS AUTOMAATICALY GENERATED (use dg_hdio.h)
 *
 *  File      : DG_HDIOC.H
 *
 *  Purpose   : Duagon Host Device IO - Access Interface
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

#ifndef DG_HDIOC_H
#define DG_HDIOC_H
#define DG_MAX_CARDS_PER_DRIVER 4

#define CARD_IS_VALID_INDEX(card_index) \
( (card_index) != DG_NO_CARD && ( \
(card_defined[0].defined && card_defined[0].index==(card_index)) ||  \
(card_defined[1].defined && card_defined[1].index==(card_index)) ||  \
(card_defined[2].defined && card_defined[2].index==(card_index)) ||  \
(card_defined[3].defined && card_defined[3].index==(card_index))   ) \
)? TRUE:FALSE


#ifndef CARD_0_TYPE
#define CARD_0_TYPE CARD_NONE
#endif
#ifndef CARD_1_TYPE
#define CARD_1_TYPE CARD_NONE
#endif
#ifndef CARD_2_TYPE
#define CARD_2_TYPE CARD_NONE
#endif
#ifndef CARD_3_TYPE
#define CARD_3_TYPE CARD_NONE
#endif


/* work out number of cards defined DG_NR_OF_CARDS_DEFINED */
#if CARD_0_TYPE==CARD_NONE
#   define __CC_0 0
#else
#   define __CC_0 1
#endif
#if CARD_1_TYPE==CARD_NONE
#   define __CC_1 0
#else
#   define __CC_1 1
#endif
#if CARD_2_TYPE==CARD_NONE
#   define __CC_2 0
#else
#   define __CC_2 1
#endif
#if CARD_3_TYPE==CARD_NONE
#   define __CC_3 0
#else
#   define __CC_3 1
#endif
#define DG_NR_OF_CARDS_DEFINED \
         __CC_0 + __CC_1 + __CC_2 + __CC_3


#define GETCARDTYPE(c_t, c_n) \
    switch((c_t)) {\
    case 0: c_n = CARD_0_TYPE;\
        break;\
    case 1: c_n = CARD_1_TYPE;\
        break;\
    case 2: c_n = CARD_2_TYPE;\
        break;\
    case 3: c_n = CARD_3_TYPE;\
        break;\
    default: c_n = CARD_NONE;\
        break;\
    }

#define INIT_CARD_DEF { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}}





/*******************************************************************************************************
 * Define initializers for the pointers in p_channel_send_package_header and p_channel_send_package_data
 * (as defined in mcs_clch.c) with dependency on the number of cards */

#if DG_NR_OF_CARDS_DEFINED==1
#define DG_CHANNEL_SEND_PACKAGE_HEADER_INIT \
{{ \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
}}

#define DG_CHANNEL_SEND_PACKAGE_DATA_INIT \
{{ \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
}}
#endif

#if DG_NR_OF_CARDS_DEFINED==2
#define DG_CHANNEL_SEND_PACKAGE_HEADER_INIT \
{{ \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
}}

#define DG_CHANNEL_SEND_PACKAGE_DATA_INIT \
{{ \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
}}
#endif

#if DG_NR_OF_CARDS_DEFINED==3
#define DG_CHANNEL_SEND_PACKAGE_HEADER_INIT \
{{ \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
}}

#define DG_CHANNEL_SEND_PACKAGE_DATA_INIT \
{{ \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
}}
#endif

#if DG_NR_OF_CARDS_DEFINED==4
#define DG_CHANNEL_SEND_PACKAGE_HEADER_INIT \
{{ \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[3].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[3].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[3].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE, \
(UNSIGNED8*)&dg_card_config[3].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE \
}}

#define DG_CHANNEL_SEND_PACKAGE_DATA_INIT \
{{ \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[0].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[1].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[2].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
},{ \
(UNSIGNED8*)&dg_card_config[3].msch.channel_send_package[0][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[3].msch.channel_send_package[1][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[3].msch.channel_send_package[2][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE, \
(UNSIGNED8*)&dg_card_config[3].msch.channel_send_package[3][0] + MCS_CLIENT_CHANNEL_DUMMY_SIZE + MCS_CLIENT_CHANNEL_HEADER_SIZE \
}}
#endif

#if DG_NR_OF_CHANNELS==1
#define CHANNEL_CONFIG_MACRO(ASSIGN,X_MACRO,INDEX,CARD) \
    switch (INDEX) { \
        case 0: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(0,0); break;\
            case 1: ASSIGN = X_MACRO(0,1); break;\
            case 2: ASSIGN = X_MACRO(0,2); break;\
            case 3: ASSIGN = X_MACRO(0,3); break;\
            default: break; };\
            break; \
        default: break; \
    }
#endif

#if DG_NR_OF_CHANNELS==2
#define CHANNEL_CONFIG_MACRO(ASSIGN,X_MACRO,INDEX,CARD) \
    switch (INDEX) { \
        case 0: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(0,0); break;\
            case 1: ASSIGN = X_MACRO(0,1); break;\
            case 2: ASSIGN = X_MACRO(0,2); break;\
            case 3: ASSIGN = X_MACRO(0,3); break;\
            default: break; };\
            break; \
        case 1: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(1,0); break;\
            case 1: ASSIGN = X_MACRO(1,1); break;\
            case 2: ASSIGN = X_MACRO(1,2); break;\
            case 3: ASSIGN = X_MACRO(1,3); break;\
            default: break; };\
            break; \
        default: break; \
    }
#endif

#if DG_NR_OF_CHANNELS==3
#define CHANNEL_CONFIG_MACRO(ASSIGN,X_MACRO,INDEX,CARD) \
    switch (INDEX) { \
        case 0: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(0,0); break;\
            case 1: ASSIGN = X_MACRO(0,1); break;\
            case 2: ASSIGN = X_MACRO(0,2); break;\
            case 3: ASSIGN = X_MACRO(0,3); break;\
            default: break; };\
            break; \
        case 1: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(1,0); break;\
            case 1: ASSIGN = X_MACRO(1,1); break;\
            case 2: ASSIGN = X_MACRO(1,2); break;\
            case 3: ASSIGN = X_MACRO(1,3); break;\
            default: break; };\
            break; \
        case 2: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(2,0); break;\
            case 1: ASSIGN = X_MACRO(2,1); break;\
            case 2: ASSIGN = X_MACRO(2,2); break;\
            case 3: ASSIGN = X_MACRO(2,3); break;\
            default: break; };\
            break; \
        default: break; \
    }
#endif

#if DG_NR_OF_CHANNELS==4
#define CHANNEL_CONFIG_MACRO(ASSIGN,X_MACRO,INDEX,CARD) \
    switch (INDEX) { \
        case 0: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(0,0); break;\
            case 1: ASSIGN = X_MACRO(0,1); break;\
            case 2: ASSIGN = X_MACRO(0,2); break;\
            case 3: ASSIGN = X_MACRO(0,3); break;\
            default: break; };\
            break; \
        case 1: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(1,0); break;\
            case 1: ASSIGN = X_MACRO(1,1); break;\
            case 2: ASSIGN = X_MACRO(1,2); break;\
            case 3: ASSIGN = X_MACRO(1,3); break;\
            default: break; };\
            break; \
        case 2: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(2,0); break;\
            case 1: ASSIGN = X_MACRO(2,1); break;\
            case 2: ASSIGN = X_MACRO(2,2); break;\
            case 3: ASSIGN = X_MACRO(2,3); break;\
            default: break; };\
            break; \
        case 3: switch (CARD) { \
            case 0: ASSIGN = X_MACRO(3,0); break;\
            case 1: ASSIGN = X_MACRO(3,1); break;\
            case 2: ASSIGN = X_MACRO(3,2); break;\
            case 3: ASSIGN = X_MACRO(3,3); break;\
            default: break; };\
            break; \
        default: break; \
    }
#endif




/* ==========================================================================
*
*  Channel Configuration (This part is not changed by the customer)
*  Here we define defaults when nothing is set in dg_conf.h
*
* ==========================================================================
*/
#define DG_CARD_0_CHANNEL_0_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_0_CHANNEL_1_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_0_CHANNEL_2_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_0_CHANNEL_3_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */

/* default mode is UE2G */
#define DG_CARD_0_CHANNEL_0_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_0_CHANNEL_1_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_0_CHANNEL_2_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_0_CHANNEL_3_MODE DG_HDCFG_CHANNEL_2G

#define DG_CARD_1_CHANNEL_0_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_1_CHANNEL_1_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_1_CHANNEL_2_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_1_CHANNEL_3_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */

/* default mode is UE2G */
#define DG_CARD_1_CHANNEL_0_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_1_CHANNEL_1_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_1_CHANNEL_2_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_1_CHANNEL_3_MODE DG_HDCFG_CHANNEL_2G

#define DG_CARD_2_CHANNEL_0_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_2_CHANNEL_1_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_2_CHANNEL_2_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_2_CHANNEL_3_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */

/* default mode is UE2G */
#define DG_CARD_2_CHANNEL_0_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_2_CHANNEL_1_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_2_CHANNEL_2_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_2_CHANNEL_3_MODE DG_HDCFG_CHANNEL_2G

#define DG_CARD_3_CHANNEL_0_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_3_CHANNEL_1_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_3_CHANNEL_2_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */
#define DG_CARD_3_CHANNEL_3_BLOCKING DG_HDCFG_BLOCKING_BLOCKING /* or DG_HDCFG_NONBLOCKING_BLOCKING */

/* default mode is UE2G */
#define DG_CARD_3_CHANNEL_0_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_3_CHANNEL_1_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_3_CHANNEL_2_MODE DG_HDCFG_CHANNEL_2G
#define DG_CARD_3_CHANNEL_3_MODE DG_HDCFG_CHANNEL_2G

/* default mode defines */



/* ==========================================================================
 *
 * UE1G register access mode for MVB CHANNELS
 *
 * ==========================================================================
 */

/* CARD_MVB_CHANNEL_NUM(card_index) "returns" NO_MVB_CHANNEL
 * if the card with "card_index" is not an MVB card */
#define NO_MVB_CHANNEL DG_MAX_CHAN_PER_CARD

/* MDFULL */
#if CARD_0_TYPE==CARD_MVB
#   if defined MVB_CHANNEL_NUM
#       if MVB_CHANNEL_NUM==0
#           undef DG_CARD_0_CHANNEL_0_MODE
#           define DG_CARD_0_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==1
#           undef DG_CARD_0_CHANNEL_1_MODE
#           define DG_CARD_0_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==2
#           undef DG_CARD_0_CHANNEL_2_MODE
#           define DG_CARD_0_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==3
#           undef DG_CARD_0_CHANNEL_3_MODE
#           define DG_CARD_0_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   endif
#endif
#if CARD_1_TYPE==CARD_MVB
#   if defined MVB_CHANNEL_NUM
#       if MVB_CHANNEL_NUM==0
#           undef DG_CARD_1_CHANNEL_0_MODE
#           define DG_CARD_1_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==1
#           undef DG_CARD_1_CHANNEL_1_MODE
#           define DG_CARD_1_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==2
#           undef DG_CARD_1_CHANNEL_2_MODE
#           define DG_CARD_1_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==3
#           undef DG_CARD_1_CHANNEL_3_MODE
#           define DG_CARD_1_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   endif
#endif
#if CARD_2_TYPE==CARD_MVB
#   if defined MVB_CHANNEL_NUM
#       if MVB_CHANNEL_NUM==0
#           undef DG_CARD_2_CHANNEL_0_MODE
#           define DG_CARD_2_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==1
#           undef DG_CARD_2_CHANNEL_1_MODE
#           define DG_CARD_2_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==2
#           undef DG_CARD_2_CHANNEL_2_MODE
#           define DG_CARD_2_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==3
#           undef DG_CARD_2_CHANNEL_3_MODE
#           define DG_CARD_2_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   endif
#endif
#if CARD_3_TYPE==CARD_MVB
#   if defined MVB_CHANNEL_NUM
#       if MVB_CHANNEL_NUM==0
#           undef DG_CARD_3_CHANNEL_0_MODE
#           define DG_CARD_3_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==1
#           undef DG_CARD_3_CHANNEL_1_MODE
#           define DG_CARD_3_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==2
#           undef DG_CARD_3_CHANNEL_2_MODE
#           define DG_CARD_3_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if MVB_CHANNEL_NUM==3
#           undef DG_CARD_3_CHANNEL_3_MODE
#           define DG_CARD_3_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   endif
#endif


/* --------------------------------------------------------------------------
 *  Channel Timeout defaults [ms]
 * --------------------------------------------------------------------------
 */
#ifndef DG_CARD_0_CHANNEL_0_READ_TIMEOUT
#define DG_CARD_0_CHANNEL_0_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_1_CHANNEL_0_READ_TIMEOUT
#define DG_CARD_1_CHANNEL_0_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_2_CHANNEL_0_READ_TIMEOUT
#define DG_CARD_2_CHANNEL_0_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_3_CHANNEL_0_READ_TIMEOUT
#define DG_CARD_3_CHANNEL_0_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_0_CHANNEL_1_READ_TIMEOUT
#define DG_CARD_0_CHANNEL_1_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_1_CHANNEL_1_READ_TIMEOUT
#define DG_CARD_1_CHANNEL_1_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_2_CHANNEL_1_READ_TIMEOUT
#define DG_CARD_2_CHANNEL_1_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_3_CHANNEL_1_READ_TIMEOUT
#define DG_CARD_3_CHANNEL_1_READ_TIMEOUT 3000
#endif

#ifndef DG_CARD_0_CHANNEL_0_WRITE_TIMEOUT
#define DG_CARD_0_CHANNEL_0_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_1_CHANNEL_0_WRITE_TIMEOUT
#define DG_CARD_1_CHANNEL_0_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_2_CHANNEL_0_WRITE_TIMEOUT
#define DG_CARD_2_CHANNEL_0_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_3_CHANNEL_0_WRITE_TIMEOUT
#define DG_CARD_3_CHANNEL_0_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_0_CHANNEL_1_WRITE_TIMEOUT
#define DG_CARD_0_CHANNEL_1_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_1_CHANNEL_1_WRITE_TIMEOUT
#define DG_CARD_1_CHANNEL_1_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_2_CHANNEL_1_WRITE_TIMEOUT
#define DG_CARD_2_CHANNEL_1_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_3_CHANNEL_1_WRITE_TIMEOUT
#define DG_CARD_3_CHANNEL_1_WRITE_TIMEOUT 1000
#endif

/* --------------------------------------------------------------------------
 *  Default timeouts for function completion [microseconds]
 * --------------------------------------------------------------------------
 */
#ifndef DG_CARD_0_CHANNEL_0_FN_TO_DEFAULT
#define DG_CARD_0_CHANNEL_0_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_1_CHANNEL_0_FN_TO_DEFAULT
#define DG_CARD_1_CHANNEL_0_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_2_CHANNEL_0_FN_TO_DEFAULT
#define DG_CARD_2_CHANNEL_0_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_3_CHANNEL_0_FN_TO_DEFAULT
#define DG_CARD_3_CHANNEL_0_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_0_CHANNEL_1_FN_TO_DEFAULT
#define DG_CARD_0_CHANNEL_1_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_1_CHANNEL_1_FN_TO_DEFAULT
#define DG_CARD_1_CHANNEL_1_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_2_CHANNEL_1_FN_TO_DEFAULT
#define DG_CARD_2_CHANNEL_1_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_3_CHANNEL_1_FN_TO_DEFAULT
#define DG_CARD_3_CHANNEL_1_FN_TO_DEFAULT 30000000
#endif


/* --------------------------------------------------------------------------
 *  Polling mode defaults
 * --------------------------------------------------------------------------
 */
#ifndef DG_CARD_0_CHANNEL_0_POLLING
#define DG_CARD_0_CHANNEL_0_POLLING FALSE
#endif
#ifndef DG_CARD_0_CHANNEL_1_POLLING
#define DG_CARD_0_CHANNEL_1_POLLING FALSE
#endif
#ifndef DG_CARD_0_CHANNEL_2_POLLING
#define DG_CARD_0_CHANNEL_2_POLLING FALSE
#endif
#ifndef DG_CARD_0_CHANNEL_3_POLLING
#define DG_CARD_0_CHANNEL_3_POLLING FALSE
#endif

#ifndef DG_CARD_1_CHANNEL_0_POLLING
#define DG_CARD_1_CHANNEL_0_POLLING FALSE
#endif
#ifndef DG_CARD_1_CHANNEL_1_POLLING
#define DG_CARD_1_CHANNEL_1_POLLING FALSE
#endif
#ifndef DG_CARD_1_CHANNEL_2_POLLING
#define DG_CARD_1_CHANNEL_2_POLLING FALSE
#endif
#ifndef DG_CARD_1_CHANNEL_3_POLLING
#define DG_CARD_1_CHANNEL_3_POLLING FALSE
#endif

#ifndef DG_CARD_2_CHANNEL_0_POLLING
#define DG_CARD_2_CHANNEL_0_POLLING FALSE
#endif
#ifndef DG_CARD_2_CHANNEL_1_POLLING
#define DG_CARD_2_CHANNEL_1_POLLING FALSE
#endif
#ifndef DG_CARD_2_CHANNEL_2_POLLING
#define DG_CARD_2_CHANNEL_2_POLLING FALSE
#endif
#ifndef DG_CARD_2_CHANNEL_3_POLLING
#define DG_CARD_2_CHANNEL_3_POLLING FALSE
#endif

#ifndef DG_CARD_3_CHANNEL_0_POLLING
#define DG_CARD_3_CHANNEL_0_POLLING FALSE
#endif
#ifndef DG_CARD_3_CHANNEL_1_POLLING
#define DG_CARD_3_CHANNEL_1_POLLING FALSE
#endif
#ifndef DG_CARD_3_CHANNEL_2_POLLING
#define DG_CARD_3_CHANNEL_2_POLLING FALSE
#endif
#ifndef DG_CARD_3_CHANNEL_3_POLLING
#define DG_CARD_3_CHANNEL_3_POLLING FALSE
#endif

/* --------------------------------------------------------------------------
 *  SERIAL CHANNEL CONFIGURATION defaults
 *
 * COM Port configurations
 * --------------------------------------------------------------------------
 */
#ifndef DG_CARD_0_SERIAL_CHANNEL_0_COM_NR
#define DG_CARD_0_SERIAL_CHANNEL_0_COM_NR 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_0_COM_NR
#define DG_CARD_1_SERIAL_CHANNEL_0_COM_NR 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_0_COM_NR
#define DG_CARD_2_SERIAL_CHANNEL_0_COM_NR 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_0_COM_NR
#define DG_CARD_3_SERIAL_CHANNEL_0_COM_NR 1
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_1_COM_NR
#define DG_CARD_0_SERIAL_CHANNEL_1_COM_NR 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_1_COM_NR
#define DG_CARD_1_SERIAL_CHANNEL_1_COM_NR 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_1_COM_NR
#define DG_CARD_2_SERIAL_CHANNEL_1_COM_NR 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_1_COM_NR
#define DG_CARD_3_SERIAL_CHANNEL_1_COM_NR 1
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_0_BAUD
#define DG_CARD_0_SERIAL_CHANNEL_0_BAUD 115200
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_0_BAUD
#define DG_CARD_1_SERIAL_CHANNEL_0_BAUD 115200
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_0_BAUD
#define DG_CARD_2_SERIAL_CHANNEL_0_BAUD 115200
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_0_BAUD
#define DG_CARD_3_SERIAL_CHANNEL_0_BAUD 115200
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_1_BAUD
#define DG_CARD_0_SERIAL_CHANNEL_1_BAUD 115200
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_1_BAUD
#define DG_CARD_1_SERIAL_CHANNEL_1_BAUD 115200
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_1_BAUD
#define DG_CARD_2_SERIAL_CHANNEL_1_BAUD 115200
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_1_BAUD
#define DG_CARD_3_SERIAL_CHANNEL_1_BAUD 115200
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_0_DATABITS
#define DG_CARD_0_SERIAL_CHANNEL_0_DATABITS 8
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_0_DATABITS
#define DG_CARD_1_SERIAL_CHANNEL_0_DATABITS 8
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_0_DATABITS
#define DG_CARD_2_SERIAL_CHANNEL_0_DATABITS 8
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_0_DATABITS
#define DG_CARD_3_SERIAL_CHANNEL_0_DATABITS 8
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_1_DATABITS
#define DG_CARD_0_SERIAL_CHANNEL_1_DATABITS 8
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_1_DATABITS
#define DG_CARD_1_SERIAL_CHANNEL_1_DATABITS 8
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_1_DATABITS
#define DG_CARD_2_SERIAL_CHANNEL_1_DATABITS 8
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_1_DATABITS
#define DG_CARD_3_SERIAL_CHANNEL_1_DATABITS 8
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_0_PARITY
#define DG_CARD_0_SERIAL_CHANNEL_0_PARITY 0
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_0_PARITY
#define DG_CARD_1_SERIAL_CHANNEL_0_PARITY 0
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_0_PARITY
#define DG_CARD_2_SERIAL_CHANNEL_0_PARITY 0
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_0_PARITY
#define DG_CARD_3_SERIAL_CHANNEL_0_PARITY 0
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_1_PARITY
#define DG_CARD_0_SERIAL_CHANNEL_1_PARITY 0
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_1_PARITY
#define DG_CARD_1_SERIAL_CHANNEL_1_PARITY 0
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_1_PARITY
#define DG_CARD_2_SERIAL_CHANNEL_1_PARITY 0
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_1_PARITY
#define DG_CARD_3_SERIAL_CHANNEL_1_PARITY 0
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_0_STOPBITS
#define DG_CARD_0_SERIAL_CHANNEL_0_STOPBITS 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_0_STOPBITS
#define DG_CARD_1_SERIAL_CHANNEL_0_STOPBITS 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_0_STOPBITS
#define DG_CARD_2_SERIAL_CHANNEL_0_STOPBITS 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_0_STOPBITS
#define DG_CARD_3_SERIAL_CHANNEL_0_STOPBITS 1
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_1_STOPBITS
#define DG_CARD_0_SERIAL_CHANNEL_1_STOPBITS 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_1_STOPBITS
#define DG_CARD_1_SERIAL_CHANNEL_1_STOPBITS 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_1_STOPBITS
#define DG_CARD_2_SERIAL_CHANNEL_1_STOPBITS 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_1_STOPBITS
#define DG_CARD_3_SERIAL_CHANNEL_1_STOPBITS 1
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_0_HANDSHAKE
#define DG_CARD_0_SERIAL_CHANNEL_0_HANDSHAKE 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_0_HANDSHAKE
#define DG_CARD_1_SERIAL_CHANNEL_0_HANDSHAKE 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_0_HANDSHAKE
#define DG_CARD_2_SERIAL_CHANNEL_0_HANDSHAKE 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_0_HANDSHAKE
#define DG_CARD_3_SERIAL_CHANNEL_0_HANDSHAKE 1
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_1_HANDSHAKE
#define DG_CARD_0_SERIAL_CHANNEL_1_HANDSHAKE 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_1_HANDSHAKE
#define DG_CARD_1_SERIAL_CHANNEL_1_HANDSHAKE 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_1_HANDSHAKE
#define DG_CARD_2_SERIAL_CHANNEL_1_HANDSHAKE 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_1_HANDSHAKE
#define DG_CARD_3_SERIAL_CHANNEL_1_HANDSHAKE 1
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_0_CTS_FLOW
#define DG_CARD_0_SERIAL_CHANNEL_0_CTS_FLOW 0
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_0_CTS_FLOW
#define DG_CARD_1_SERIAL_CHANNEL_0_CTS_FLOW 0
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_0_CTS_FLOW
#define DG_CARD_2_SERIAL_CHANNEL_0_CTS_FLOW 0
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_0_CTS_FLOW
#define DG_CARD_3_SERIAL_CHANNEL_0_CTS_FLOW 0
#endif
#ifndef DG_CARD_0_SERIAL_CHANNEL_1_CTS_FLOW
#define DG_CARD_0_SERIAL_CHANNEL_1_CTS_FLOW 0
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_1_CTS_FLOW
#define DG_CARD_1_SERIAL_CHANNEL_1_CTS_FLOW 0
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_1_CTS_FLOW
#define DG_CARD_2_SERIAL_CHANNEL_1_CTS_FLOW 0
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_1_CTS_FLOW
#define DG_CARD_3_SERIAL_CHANNEL_1_CTS_FLOW 0
#endif
#endif /* DG_HDIOC_H */
