/* ==========================================================================
 *
 *  File      : DG_REG1G.C
 *
 *  Purpose   : Duagon Host Device IO
 *              UE1G access implementation
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

/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */
#include "dg_hdio.h"
#include "dg_hdreg.h"
#include "dg_reg1g.h"
#include "osl.h"
#if (DG_IO_MODE==DG_HDCFG_USB)
#include "dg_hdusb.h"
#endif
#include "dg_hdpar.h"


#if (1 == HDIO_DEBUG)
#define hdreg1_debug_printf osl_printf
#else
#define hdreg1_debug_printf dummy_printf
DG_DECL_LOCAL void dummy_printf(DG_DECL_CONST DG_CHAR8* nothing,...) { (void)nothing; } /* dummy printf to avoid warnings */
#endif

/* ==========================================================================
 *
 *  Private Procedures (forward declarations)
 *
 * ==========================================================================
 */

void dg_reg1g_set_perf_callbacks(PerfFunc write, PerfFunc read_d, PerfFunc read_s, PerfFuncU8 polling, DG_U8 card_index) {
    dg_card_config[card_index].p1g.write_func = write;
    dg_card_config[card_index].p1g.read_d_func = read_d;
    dg_card_config[card_index].p1g.read_s_func = read_s;
    dg_card_config[card_index].p1g.is_polling_func = polling;
}

DG_DECL_LOCAL
DG_BOOL
dg_reg_tx_status_check
(
        REG_SIZE* addr,
        DG_U8 card_index
);

DG_DECL_LOCAL
DG_BOOL
dg_reg_rx_status_check
(
        REG_SIZE* addr,
        DG_U8 card_index
);

#if (DG_IO_MODE==DG_HDCFG_USB)
DG_DECL_LOCAL
DG_RESULT
dg_reg_wait_for_tx_status
(
        REG_SIZE* addr,
        DG_U32 timeout,
        DG_U8 card_index
);

DG_DECL_LOCAL
DG_RESULT
dg_reg_wait_for_rx_status
(
        REG_SIZE* addr,
        DG_U32 timeout,
        DG_U8 card_index
);
#endif
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
)
{
    DG_RESULT res = DG_OK;
    UE1G_REGISTER_MAP* base = (UE1G_REGISTER_MAP*)GET_REG_BASE(card_index,channel_num);
    OSL_TIMER  t1;
    DG_U32 bytes_read = 0;
    DG_BOOL status;
#ifdef USE_32BIT_ACCESS
    DG_U32 data32;
    DG_S32 i;
#endif

    if (0 == *size)
        return DG_OK;

#if (DG_IO_MODE==DG_HDCFG_USB)
    if ( DG_CARD_IO_MODE(card_index)  == DG_HDCFG_USB ) {
        if (DG_HDCFG_BLOCKING_BLOCKING == dg_card_config[card_index].dg_hdio_configs[channel_num].blocking) {
            hdreg1_debug_printf("dg_reg_receive_1g_blocking\n");
            if (DG_ERROR_TIMEOUT == dg_reg_wait_for_rx_status((REG_SIZE*)(&(base->status)),dg_card_config[card_index].dg_hdio_configs[channel_num].write_timeout, card_index))
                return DG_ERROR_TIMEOUT;
        } else {
            hdreg1_debug_printf("dg_reg_receive_1g_non_blocking\n");
            if (!dg_reg_rx_status_check((REG_SIZE*)(&(base->status)), card_index))
                return DG_ERROR_TIMEOUT;
        }
        res = osl_usb_hal_read((REG_SIZE*)(&(base->io)), size, rxBuff, card_index);
    }
#endif

    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {

        osl_start_timer( &t1 );

        base = (UE1G_REGISTER_MAP*)GET_REG_BASE(card_index,channel_num);

         /* waits for all chars available (bit is 1 when all bytes are in the FIFO) */
         while (!(status = dg_reg_rx_status_check((REG_SIZE*)((REG_SIZE*)(&base->status)), card_index)) && osl_check_timer(&t1 , dg_card_config[card_index].dg_hdio_configs[channel_num].read_timeout ) ){
             /* busy wait */
         }
         if (status) {
             bytes_read = 0;
             while(bytes_read<(*size)){
#ifdef USE_32BIT_ACCESS
                if (((*size) - bytes_read) >= 4){ /* at least 4 bytes left -> do 32 bit word access */
                    data32 = osl_par_hal_read32((REG_SIZE*)(&(base->io)));
                    for (i=0; i<4; i++){
                        *rxBuff = (DG_U8)((data32 >> i*8) & 0xff);
                        rxBuff++;
                        bytes_read++;
                    }
                } else {
                    *rxBuff = osl_par_hal_read8((REG_SIZE*)(&(base->io)));
                    rxBuff++;
                    bytes_read++;
                }
#else
			
#ifdef SERVER_BOARD
			
			osl_start_timer( &t1 );

        	base = (UE1G_REGISTER_MAP*)GET_REG_BASE(card_index,channel_num);

        	/* waits for all chars available (bit is 1 when all bytes are in the FIFO) */
            while (!(status = dg_reg_rx_status_check((REG_SIZE*)((REG_SIZE*)(&base->status)), card_index)) && osl_check_timer(&t1 , dg_card_config[card_index].dg_hdio_configs[channel_num].read_timeout ) ){
        	 /* busy wait */
        	 }
			 
#endif
	
                *rxBuff = osl_par_hal_read8((REG_SIZE*)(&(base->io)));
                rxBuff++;
                bytes_read++;
#endif
                if (dg_card_config[card_index].p1g.read_d_func!=NULL) {
                    dg_card_config[card_index].p1g.read_d_func(channel_num, card_index);
                }
             }
         } else {
             res = DG_ERROR_TIMEOUT;
         }

    }
    return res;
}

DG_DECL_PUBLIC
DG_RESULT
dg_reg_transmit_1g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* txBuff,
        DG_U8 card_index
)
{
    DG_RESULT res = DG_OK;
    UE1G_REGISTER_MAP* base = (UE1G_REGISTER_MAP*)GET_REG_BASE(card_index,channel_num);
    DG_U32 curr_size;
    OSL_TIMER  t1;
    DG_BOOL status;
#ifdef USE_32BIT_ACCESS
    DG_U32 data32;
    DG_S32 i;
#endif

#if (DG_IO_MODE==DG_HDCFG_USB)
    if  ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        if (DG_HDCFG_BLOCKING_BLOCKING == dg_card_config[card_index].dg_hdio_configs[channel_num].blocking) {
            if (DG_ERROR_TIMEOUT == dg_reg_wait_for_tx_status((REG_SIZE*)(&(base->status)), dg_card_config[card_index].dg_hdio_configs[channel_num].write_timeout, card_index)) {
                return DG_ERROR_TIMEOUT;
            }
        } else {
            if (!dg_reg_tx_status_check((REG_SIZE*)(&(base->status)), card_index)){
                return DG_ERROR_TIMEOUT;
            }
        }
        res = osl_usb_hal_write((REG_SIZE*)(&(base->io)), size, txBuff, card_index);
    }
#endif

    if  ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {

        osl_start_timer( &t1 );

        /* waits for all chars available (bit is 1 when all bytes are in the FIFO) */
        while (!(status = dg_reg_tx_status_check((REG_SIZE*)((REG_SIZE*)(&base->status)), card_index)) && osl_check_timer(&t1 , dg_card_config[card_index].dg_hdio_configs[channel_num].write_timeout ) ){
            /* busy wait */
        }
        if (!status)
            return DG_ERROR_TIMEOUT;
        curr_size = 0;
        while(curr_size<(*size)){
#ifdef USE_32BIT_ACCESS
            if (((*size) - curr_size) >= 4){ /* at least 4 bytes left -> do 32 bit word access */
                data32 = 0;
                for (i=0; i<4; i++){
                    data32 = data32 | (((DG_U32)*txBuff << i*8) & (0xff << i*8));
                    txBuff++;
                    curr_size++;
                }
                osl_par_hal_write32((REG_SIZE*)(&(base->io)), data32);
            } else {
                osl_par_hal_write8((REG_SIZE*)(&(base->io)),*txBuff);
                txBuff++;
                curr_size++;
            }
#else
            osl_par_hal_write8((REG_SIZE*)(&(base->io)),*txBuff);
            txBuff++;
            curr_size++;
#endif
            if (dg_card_config[card_index].p1g.write_func!=NULL) {
                dg_card_config[card_index].p1g.write_func(channel_num, card_index);
            }
        }

    }
    return res;
}

/* ==========================================================================
 *
 *  Private Procedures (implementation)
 *
 * ==========================================================================
 */

DG_DECL_LOCAL
DG_BOOL
dg_reg_tx_status_check
(
        REG_SIZE* addr,
        DG_U8 card_index
)
{
    DG_BOOL res = DG_OK;
    hdreg1_debug_printf("dg_reg1_tx_status_check 0x%08X\n", (DG_U32)addr);
#ifdef USE_OLD_UE1G_REGISTER_MAP
    res = (DG_BOOL)((hd_reg_get_status(addr, card_index) & 0x80) != 0);
#else
    res = (DG_BOOL)((hd_reg_get_status(addr, card_index) & 0x40) != 0);
#endif
    if (dg_card_config[card_index].p1g.read_s_func!=NULL) {
        dg_card_config[card_index].p1g.read_s_func(MVB_PROTOCOL_CHANNEL, card_index);
    }
    return res;
}


/* returns true if status is OK */
/* actually no: this returns false if status is OK*/
DG_DECL_LOCAL
DG_BOOL
dg_reg_rx_status_check
(
        REG_SIZE* addr,
        DG_U8 card_index
)
{
    DG_BOOL res = (DG_BOOL)((hd_reg_get_status(addr, card_index) & 0x01) != 0);
    if (dg_card_config[card_index].p1g.read_s_func!=NULL) {
        dg_card_config[card_index].p1g.read_s_func(MVB_PROTOCOL_CHANNEL, card_index);
    }
    return res;
}

#if (DG_IO_MODE==DG_HDCFG_USB)
DG_DECL_LOCAL
DG_RESULT
dg_reg_wait_for_rx_status
(
        REG_SIZE* addr,
        DG_U32 timeout,
        DG_U8 card_index
)
{
    OSL_TIMER t1;

    hdreg1_debug_printf("dg_reg1_wait_for_status\n");
    osl_start_timer(&t1);
    while (!dg_reg_rx_status_check(addr, card_index)) {
        if (!osl_check_timer(&t1, timeout)) {
            hdreg1_debug_printf("dg_reg1_wait_for_status status TIMEOUT\n");
            return DG_ERROR_TIMEOUT;
        }
    }
    hdreg1_debug_printf("dg_reg1_wait_for_status status OK\n");
    return DG_OK;
}


DG_DECL_LOCAL
DG_RESULT
dg_reg_wait_for_tx_status
(
        REG_SIZE* addr,
        DG_U32 timeout,
        DG_U8 card_index
)
{
    OSL_TIMER t1;

    hdreg1_debug_printf("dg_reg1_wait_for_status\n");
    osl_start_timer(&t1);
    while (!dg_reg_tx_status_check(addr, card_index)) {
        if (!osl_check_timer(&t1, timeout)) {
            return DG_ERROR_TIMEOUT;
        }
    }
    return DG_OK;
}

#endif
