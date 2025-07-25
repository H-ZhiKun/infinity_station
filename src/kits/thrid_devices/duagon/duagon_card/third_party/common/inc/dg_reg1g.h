/* ==========================================================================
 *
 *  File      : DG_REG1G.H
 *
 *  Purpose   : Duagon Host Device IO
 *              UE1G access definition
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

#ifndef DG_REG1G_H
#define DG_REG1G_H

/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */
#include "dg_hdio.h"


#ifndef USE_OLD_UE1G_REGISTER_MAP
	typedef struct
	{
  	    REG_SIZE   io;  /* Read and Write register (BASE Address)  */
  	    REG_SIZE   undefined;
  	    REG_SIZE   reg_undefined;/* Undefined                      */
  	    REG_SIZE   reg_undefined1;/* Undefined                     */
  	    REG_SIZE   reg_undefined2;/* Undefined                     */
   	    REG_SIZE   status;
	}   UE1G_REGISTER_MAP;
#else
    typedef struct
    {
        REG_SIZE   io;  /* Read and Write register (BASE Address)  */
        REG_SIZE   status;
        REG_SIZE   undefined;
        REG_SIZE   reg_undefined;/* Undefined                      */
        REG_SIZE   reg_undefined1;/* Undefined                     */
        REG_SIZE   reg_undefined2;/* Undefined                     */
    }   UE1G_REGISTER_MAP;
#endif


typedef struct
{
    UE1G_REGISTER_MAP*    reg_cch_base;
    DG_U32                reg_timeout;
    DG_U32                fifo_size;
}   UE1G_DESC_STRUCT_UART;

/* ==========================================================================
 *
 *  Public Procedures (general)
 *
 * ==========================================================================
 */
DG_DECL_PUBLIC
DG_RESULT
dg_reg_receive_1g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* rxBuff,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_reg_transmit_1g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* txBuff,
        DG_U8 card_index
);

#endif /* DG_REG1G_H */
