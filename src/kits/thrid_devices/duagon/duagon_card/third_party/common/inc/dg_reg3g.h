/* ==========================================================================
 *
 *  File      : DG_REG3G.H
 *
 *  Purpose   : Duagon Host Device IO
 *              UE3G access definition
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

#ifndef DG_REG3G_H
#define DG_REG3G_H

/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */
#include "dg_hdio.h"

typedef struct
{
    REG_SIZE   reg_undefined;/* Undefined   (Attention used from reg1g)  */
    REG_SIZE   reg_undefined1;/* Undefined                               */
    REG_SIZE   reg_undefined2;/* Undefined                               */
    REG_SIZE   reg_undefined3;/* Undefined                               */
    REG_SIZE   reg_undefined4;/* Undefined                               */
    REG_SIZE   reg_undefined5;/* Undefined  (Attention used from reg1g)  */
    REG_SIZE   io;  /* Read and Write register (BASE Address)  */
    REG_SIZE   status;
   
}   UE3G_REGISTER_MAP;

typedef struct
{
    UE3G_REGISTER_MAP*    reg_cch_base;
    DG_U32                reg_timeout;
    DG_U32                fifo_size;
}   UE3G_DESC_STRUCT_UART;

/* ==========================================================================
 *
 *  Public Procedures (general)
 *
 * ==========================================================================
 */
DG_DECL_PUBLIC
DG_RESULT
dg_reg_receive_3g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* rxBuff,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_reg_transmit_3g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* txBuff,
        DG_U8 card_index
);

#endif /* DG_REG3G_H */
