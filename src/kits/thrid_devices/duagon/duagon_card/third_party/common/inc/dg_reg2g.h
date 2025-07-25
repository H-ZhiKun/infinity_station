/* ==========================================================================
 *
 *  File      : DG_REG2G.H
 *
 *  Purpose   : Duagon Host Device IO -
 *              UE2G Register access interface
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

#ifndef DG_REG2G_H
#define DG_REG2G_H

/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */
#include "dg_hdio.h"


#define REG_LSR_LOOPBACK            0x02

typedef struct
{
    REG_SIZE   reg_rbr_thr;  /* Read and Write register (BASE Address)  */
    REG_SIZE   reg_rbr;      /* Read current value without consumation  */
    REG_SIZE   reg_rsr;      /* How may bytes the Receive FIFO contain  */
    REG_SIZE   reg_tsr;      /* The Bytes in the Transmit FIFO          */
    REG_SIZE   reg_ccr;      /* Channel configuration register          */
    REG_SIZE   reg_lsr;      /* Line Status (Handshaking)               */
    REG_SIZE   reg_undefined;/* Undefined                               */
    REG_SIZE   reg_fsr;      /* FIFO size (value << 7 = # Bytes)        */
}   UE2G_REGISTER_MAP;

typedef struct
{
    UE2G_REGISTER_MAP*       reg_cch_base;
    DG_U32                reg_rbr_thr;
    DG_U32                reg_lsr;
    DG_U32                reg_size;
    DG_U32                reg_timeout;
    DG_U32                fifo_size;
    OSL_MUTEX             channel_mutex;
}   UE2G_DESC_STRUCT_UART;


/* ==========================================================================
 *
 *  Public Procedures (general)
 *
 * ==========================================================================
 */
DG_DECL_PUBLIC
DG_RESULT
dg_reg_do_init_sanity_check_2g
(
		DG_U8 channel_num,
	    DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_reg_receive_2g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* rxBuff,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_reg_transmit_2g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* txBuff,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_reg_set_loopback_2g
(
		DG_U8 channel_num,
	    DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_reg_release_loopback_2g
(
		DG_U8 channel_num,
	    DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_reg_do_empty_full_sanity_check_2g
(
		DG_U8 channel_num,
		DG_U8 card_index
);

#endif /* DG_REG2G_H */
