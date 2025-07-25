/* ==========================================================================
 *
 *  File      : DG_REG2G.C
 *
 *  Purpose   : Duagon Host Device IO -
 *              Implementation of the UE2G register access
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
#include "dg_reg2g.h"
#include "osl.h"
#if (DG_IO_MODE==DG_HDCFG_USB)
#include "dg_hdusb.h"
#endif
#include "dg_hdpar.h"


#if (1 == HDIO_DEBUG)
#define hdreg2_debug_printf osl_printf
#else
#define hdreg2_debug_printf dummy_printf
DG_DECL_LOCAL void dummy_printf(DG_DECL_CONST DG_CHAR8* nothing,...) { (void)nothing; } /* dummy printf to avoid warnings */
#endif

/* ==========================================================================
 *
 *  Private Procedures (general)
 *
 * ==========================================================================
 */

void dg_reg2g_set_perf_callbacks(PerfFunc write, PerfFunc read_d, PerfFunc read_s, PerfFuncU8 polling, DG_U8 card_index) {
    dg_card_config[card_index].p2g.write_func = write;
    dg_card_config[card_index].p2g.read_d_func = read_d;
    dg_card_config[card_index].p2g.read_s_func = read_s;
    dg_card_config[card_index].p2g.is_polling_func = polling;
}

DG_U32
dg_reg_get_bytes_able_to_send
(
        UE2G_REGISTER_MAP*    base,
        DG_U8 channel_num,
        DG_U8 card_index
)
{
    DG_U32 freeBytes = 0;

    /* Read the Transmit Buffer State */
    freeBytes = hd_reg_get_status(&(base->reg_tsr), card_index);
    if (dg_card_config[card_index].p2g.read_s_func!=NULL) {
        dg_card_config[card_index].p2g.read_s_func(channel_num, card_index);
    }
    /* The encoding of TSR says that 0xFF means absolutely empty */
    if(0xFF == freeBytes){
        freeBytes = (GET_REG_CONFIG(card_index,channel_num))->fifo_size;
    }

    return freeBytes;
} /* osl_get_bytes_able_to_send */


DG_U32
dg_reg_get_nr_bytes_received
(
    UE2G_REGISTER_MAP*    base,
    DG_U8 channel_num,
    DG_U8 card_index
)
{
    DG_U32            nrBytes = 0;

    /* Read the Receive Buffer State */
    nrBytes = hd_reg_get_status(&(base->reg_rsr), card_index);
    if (dg_card_config[card_index].p2g.read_s_func!=NULL) {
        dg_card_config[card_index].p2g.read_s_func(channel_num, card_index);
    }
    /* The encoding of RSR says that 0xFF means absolutely full */
    if(0xFF == nrBytes){
        nrBytes = (GET_REG_CONFIG(card_index,channel_num))->fifo_size;
    }

    return nrBytes;
} /* osl_get_nr_bytes_received */

DG_RESULT
dg_reg_do_empty_full_sanity_check_2g(DG_U8 channel_num, DG_U8 card_index)
{
    UE2G_REGISTER_MAP*  base = (UE2G_REGISTER_MAP*)GET_REG_BASE(card_index,channel_num);
    DG_U8 getLSR;
    DG_U8 getFSR;
    getLSR = (DG_U8)(hd_reg_get_status(&base->reg_lsr, card_index) & (DG_U8)0xE2);
    if (!((getLSR == 0x60) || (getLSR == 0x40)))
    {
        hdreg2_debug_printf("\nLSR register not correct (lsr=%08x)\n",getLSR);
        return DG_ERROR_REGISTER_NOT_CORRECT;
    }
    if (dg_card_config[card_index].p2g.read_s_func!=NULL) {
        dg_card_config[card_index].p2g.read_s_func(channel_num, card_index);
    }
    getFSR = hd_reg_get_status(&base->reg_fsr, card_index);
    if ( ((GET_REG_CONFIG(card_index,channel_num))->fifo_size) != (DG_U32)(FIFO_SIZE( getFSR )) )
    {
        hdreg2_debug_printf("\nFSR register not correct (fsr=%08x, expected=%08x)\n",
                getFSR,(GET_REG_CONFIG(card_index,channel_num))->fifo_size);
        return DG_ERROR_REGISTER_NOT_CORRECT;
    }
    if (dg_card_config[card_index].p2g.read_s_func!=NULL) {
        dg_card_config[card_index].p2g.read_s_func(channel_num, card_index);
    }
    return DG_OK;
}

/* ==========================================================================
 *
 *  Public Procedures (general)
 *
 * ==========================================================================
 */


DG_DECL_PUBLIC
DG_RESULT
dg_reg_do_init_sanity_check_2g(DG_U8 channel_num, DG_U8 card_index)
{
    UE2G_REGISTER_MAP*  base = (UE2G_REGISTER_MAP*)GET_REG_BASE(card_index,channel_num);
    hdreg2_debug_printf("======\n reg_rbr_thr %0X\n reg_rbr %0X\n reg_rsr %0X\n reg_tsr %0X\n reg_ccr %0X\n reg_lsr %0X\n reg_undefined %0X\n reg_fsr %0X\n ",
                    hd_reg_get_status(&base->reg_rbr_thr, card_index),
                    hd_reg_get_status(&base->reg_rbr, card_index),
                    hd_reg_get_status(&base->reg_rsr, card_index),
                    hd_reg_get_status(&base->reg_tsr, card_index),
                    hd_reg_get_status(&base->reg_ccr, card_index),
                    hd_reg_get_status(&base->reg_lsr, card_index),
                    hd_reg_get_status(&base->reg_undefined, card_index),
                    hd_reg_get_status(&base->reg_fsr, card_index)
                    );

    if ( (hd_reg_get_status(&base->reg_lsr, card_index) & (DG_U8)0xE2) != 0x60 )
    {
        hdreg2_debug_printf("\nLSR register not ready\n");
        return DG_ERROR_OSL_NOT_READY;
    }
    if ( hd_reg_get_status(&base->reg_rsr, card_index) != 0x00 )
    {
        hdreg2_debug_printf("\nRSR register not ready\n");
        return DG_ERROR_OSL_NOT_READY;
    }
    if ( hd_reg_get_status(&base->reg_tsr, card_index) != 0xFF )
    {
        hdreg2_debug_printf("\nTSR register not ready\n");
        return DG_ERROR_OSL_NOT_READY;
    }
    return DG_OK;
}


DG_DECL_PUBLIC
DG_RESULT
dg_reg_receive_2g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* rxBuff,
        DG_U8 card_index
)
{
    UE2G_REGISTER_MAP*  base = (UE2G_REGISTER_MAP*)GET_REG_BASE(card_index,channel_num);
    DG_S32 *function_time_out = LOCATE_CFG_FTO(card_index,channel_num);
    DG_S32 *function_start_time = LOCATE_CFG_FST(card_index,channel_num);
    DG_U8* rxBuffEnd = rxBuff + (*size);
    DG_U8* recvByte = rxBuff;
    DG_U8* portionEnd;
    DG_RESULT             result = DG_OK;
#ifdef USE_32BIT_ACCESS
    DG_U32 data32;
    DG_S32 i;
#endif

    portionEnd = rxBuff + dg_reg_get_nr_bytes_received(base, channel_num, card_index);
    if ((DG_U32)(portionEnd - rxBuff) < *size && DG_OK == result &&
        DG_HDCFG_BLOCKING_NONBLOCKING == (GET_REG_CONFIG(card_index,channel_num))->dg_hdio_common_config.blocking &&
        (DG_U32)(portionEnd - rxBuff) < (GET_REG_CONFIG(card_index,channel_num))->fifo_size)
    {
        result = DG_ERROR_HDIO_RECV_WOULD_BLOCK; /* not enough data for a non-blocking call */
        hdreg2_debug_printf("TIMEOUT\n");
    }

    while (rxBuffEnd > recvByte && DG_OK == result) {
        portionEnd = recvByte + dg_reg_get_nr_bytes_received(base, channel_num, card_index);
        if ( portionEnd == recvByte )
        {
            result = dg_reg_do_empty_full_sanity_check_2g(channel_num, card_index);
            if ( result != DG_OK )break;
        }
        if(portionEnd > rxBuffEnd) portionEnd = rxBuffEnd;
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
            while ( recvByte < portionEnd)
            {
#ifdef USE_32BIT_ACCESS
                if ((portionEnd - recvByte) >= 4){ /* at least 4 bytes left -> do 32 bit word access */
                    data32 = osl_par_hal_read32(&base->reg_rbr_thr);
                    for (i=0; i<4; i++){
                        *recvByte = (DG_U8)((data32 >> i*8) & 0xff);
                        recvByte++;
                    }
                } else {
                    *recvByte = osl_par_hal_read8(&base->reg_rbr_thr);
                    recvByte++;
                }
#else
                *recvByte = osl_par_hal_read8(&base->reg_rbr_thr);
                recvByte++;
#endif
                if (dg_card_config[card_index].p2g.read_d_func!=NULL) {
                    dg_card_config[card_index].p2g.read_d_func(channel_num, card_index);
                }
            }
        }
#if (DG_IO_MODE==DG_HDCFG_USB)
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
            DG_U32 nrReadPortion = portionEnd - recvByte;
            if (0!=nrReadPortion) {
                     result = osl_usb_hal_read(&base->reg_rbr_thr, &nrReadPortion, recvByte, card_index);
                     recvByte += nrReadPortion;
            }
        }
#endif
        if (DG_OK == result) {
            if ( osl_get_us_time_stamp() - *function_start_time > *function_time_out )
            {
                hdreg2_debug_printf("dg_reg_receive_2g error: function exceeds time limit \n");
                result = DG_ERROR_FUNCTION_TIMED_OUT;
            }
        }
    }

    hdreg2_debug_printf("dg_reg_receive_2g result = %d\n", result);

    return result;
}


DG_DECL_PUBLIC
DG_RESULT
dg_reg_transmit_2g
(
        DG_U8 channel_num,
        DG_U32* size,
        DG_U8* txBuff,
        DG_U8 card_index
)
{
     UE2G_REGISTER_MAP*  base = GET_REG_BASE(card_index,channel_num);
     DG_S32 *function_time_out = LOCATE_CFG_FTO(card_index,channel_num);
     DG_S32 *function_start_time = LOCATE_CFG_FST(card_index,channel_num);
     DG_U8* txBuffEnd = txBuff + (*size);
     DG_U8* sendByte = txBuff;
     DG_U8* portionEnd;
     DG_RESULT             result = DG_OK;
#ifdef USE_32BIT_ACCESS
     DG_U32 data32;
     DG_S32 i;
#endif

     /* is the device still connected? */
     if ( (hd_reg_get_status(&(base->reg_lsr), card_index)&0xdc)!=0x40 ) {   /* bits must be 01x000xx */
         return DG_ERROR_COMM_FAIL;
     }

     hdreg2_debug_printf("dg_reg_transmit_2g size = %d\n", (*size));
     while (txBuffEnd > sendByte && DG_OK == result)
     {
        portionEnd = sendByte + dg_reg_get_bytes_able_to_send(base, channel_num, card_index);
        if ( portionEnd == sendByte ) {
            /* check register set if fifo is full */
            result = dg_reg_do_empty_full_sanity_check_2g(channel_num, card_index);
            if ( result != DG_OK )break;
        }
        if(portionEnd > txBuffEnd) 
            portionEnd = txBuffEnd;
            
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
             while ( sendByte < portionEnd)
             {
#ifdef USE_32BIT_ACCESS
                if ((portionEnd - sendByte) >= 4){ /* at least 4 bytes left -> do 32 bit word access */
                    data32 = 0;
                    for (i=0; i<4; i++){
                        data32 = data32 | (((DG_U32)*sendByte << i*8) & (0xff << i*8));
                        sendByte++;
                    }
                    osl_par_hal_write32(&base->reg_rbr_thr, data32);
                } else {
                    osl_par_hal_write8(&base->reg_rbr_thr,*sendByte);
                    sendByte++;
                }
#else
                osl_par_hal_write8(&base->reg_rbr_thr,*sendByte);
                sendByte++;
#endif
                if (dg_card_config[card_index].p2g.write_func!=NULL) {
                    dg_card_config[card_index].p2g.write_func(channel_num, card_index);
                }
             }
        }
#if (DG_IO_MODE==DG_HDCFG_USB)
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
            DG_U32 nrWritePortion = portionEnd - sendByte;
            if (0 != nrWritePortion) {
                result = osl_usb_hal_write(&base->reg_rbr_thr, &nrWritePortion, sendByte, card_index);
                sendByte += nrWritePortion;
            }
        }
#endif
        if (DG_OK == result) {
             if ( osl_get_us_time_stamp() - *function_start_time > *function_time_out )
             {
                 hdreg2_debug_printf("dg_reg_transmit_2g error: function exceeds time limit \n");
                 result = DG_ERROR_FUNCTION_TIMED_OUT;
             }
         }
     }
     hdreg2_debug_printf("dg_reg_transmit_2g result = %d\n", result);
     return result;
}

DG_RESULT
dg_reg_loopback_2g_start
(
    UE2G_REGISTER_MAP*  base,
    DG_U8* data,
    DG_U8 card_index
)
{
    DG_RESULT     result = DG_OK;
#if (DG_IO_MODE==DG_HDCFG_USB)
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        DG_U32           size = 1;
        result = osl_usb_hal_read(&base->reg_lsr, &size, data, card_index);
    }
#endif
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        *data = osl_par_hal_read8(&base->reg_lsr);
    }
    return result;
}


DG_DECL_PUBLIC
DG_RESULT
dg_reg_set_loopback_2g
(
    DG_U8 channel_num,
    DG_U8 card_index
)
{
    UE2G_REGISTER_MAP*  base = GET_REG_BASE(card_index,channel_num);
    DG_RESULT     result = DG_OK;
    DG_U8            data = 0;

    /* Read the LSR register */
    result = dg_reg_loopback_2g_start(base, &data, card_index);
    /* Release the Loopback bit in the LSR register */
    data = (DG_U8)(data | REG_LSR_LOOPBACK);
    /* Write the LSR register back */
#if (DG_IO_MODE==DG_HDCFG_USB)
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        DG_U32           size = 1;
        result = osl_usb_hal_write(&base->reg_lsr,&size,&data, card_index);
    }
#endif
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        osl_par_hal_write8(&base->reg_lsr,data);
    }
    return result;
}

DG_DECL_PUBLIC
DG_RESULT
dg_reg_release_loopback_2g
(
    DG_U8 channel_num,
    DG_U8 card_index
)
{
    UE2G_REGISTER_MAP*  base = GET_REG_BASE(card_index,channel_num);
    DG_RESULT     result = DG_OK;
    DG_U8            data = 0;

    /* Read the LSR register */
#if (DG_IO_MODE==DG_HDCFG_USB)
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        DG_U32 size = 1;
        result = osl_usb_hal_read(&base->reg_lsr, &size, &data, card_index);
    }
#endif
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        data = osl_par_hal_read8(&base->reg_lsr);
    }
    /* Release the Loopback bit in the LSR register */
    data = (DG_U8)(data & ~REG_LSR_LOOPBACK);
    /* Write the LSR register back */
#if (DG_IO_MODE==DG_HDCFG_USB)
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        DG_U32 size = 1;
        result = osl_usb_hal_write(&base->reg_lsr,&size,&data, card_index);
    }
#endif
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        osl_par_hal_write8(&base->reg_lsr,data);
    }
    return result;
}

