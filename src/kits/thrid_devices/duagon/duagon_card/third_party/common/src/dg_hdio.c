/* ==========================================================================
 *
 *  File      : DG_HDIO.C
 *
 *  Purpose   : Host Device IO Communication
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
#include "dg_hdio.h"
#include "osl.h"
#include "dg_hdreg.h"

#if (DG_IO_MODE==DG_HDCFG_SERIAL)
#include "dg_hdstm.h"
#endif

#if (1 == HAVE_PRINTF)
#include <stdio.h>     /* printf        */
#include <stdarg.h>    /* vprintf       */
#endif

#if (1 == HDIO_DEBUG)
#define hdio_debug_printf osl_printf
#else
#define hdio_debug_printf dummy_printf
DG_DECL_LOCAL void dummy_printf(DG_DECL_CONST DG_CHAR8* nothing,...) { (void)nothing; } /* dummy printf to avoid warnings */
#endif

DG_CARD_DEFINED card_defined[DG_MAX_CARDS_PER_DRIVER]=INIT_CARD_DEF; /* one for each posible cards */
DG_CARD_CONFIG dg_card_config[DG_NR_OF_CARDS_DEFINED]; /* one for each card used */


DG_DECL_LOCAL
DG_RESULT
init_channels(
        DG_U8 card_index
)
{
    DG_S32 chi, cdi, cd_nbr;
    DG_RESULT res = DG_ERROR;

    if (dg_card_config[card_index].chan_is_initialized) return DG_OK;
    
    /* common configuration */
    cdi = card_index;
    cd_nbr = dg_card_config[cdi].card_number;
    for (chi=0;chi<DG_NR_OF_CHANNELS;chi++) {
        CHANNEL_CONFIG_MACRO(dg_card_config[cdi].dg_hdio_configs[chi].channel_type,DG_CARD_Y_CHANNEL_X_MODE,chi,cd_nbr)
        dg_card_config[cdi].dg_hdio_configs[chi].is_initialized = DG_HDIO_NOT_INITIALIZED;
        CHANNEL_CONFIG_MACRO(dg_card_config[cdi].dg_hdio_configs[chi].read_timeout,DG_CARD_Y_CHANNEL_X_READ_TIMEOUT,chi,cd_nbr)
        CHANNEL_CONFIG_MACRO(dg_card_config[cdi].dg_hdio_configs[chi].write_timeout,DG_CARD_Y_CHANNEL_X_WRITE_TIMEOUT,chi,cd_nbr)
        CHANNEL_CONFIG_MACRO(dg_card_config[cdi].dg_hdio_configs[chi].blocking,DG_CARD_Y_CHANNEL_X_BLOCKING,chi,cd_nbr)
        CHANNEL_CONFIG_MACRO(dg_card_config[cdi].dg_hdio_configs[chi].is_polling,DG_CARD_Y_CHANNEL_X_POLLING,chi,cd_nbr)
        CHANNEL_CONFIG_MACRO(dg_card_config[cdi].dg_hdio_configs[chi].fn_timeout_default,DG_CARD_Y_CHANNEL_X_FN_TO_DEFAULT,chi,cd_nbr)
        CHANNEL_CONFIG_MACRO(dg_card_config[cdi].dg_hdio_configs[chi].function_timeout,DG_CARD_Y_CHANNEL_X_FN_TO_DEFAULT,chi,cd_nbr)
        dg_card_config[cdi].dg_hdio_configs[chi].handle = NULL;
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL || DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        res = dg_hdreg_install(card_index);
    }
#if (DG_IO_MODE==DG_HDCFG_SERIAL)
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_SERIAL ) {
        res = dg_hdstm_install(card_index);
    }
#endif
    if ( res == DG_OK )
        dg_card_config[card_index].chan_is_initialized = TRUE;

    return res;
}

DG_RESULT
dg_hdio_init(
    DG_U8 channel_num,
    DG_U8 card_index
)
{
    DG_RESULT res = DG_OK;
    DG_RESULT res_mutex = DG_OK;

    hdio_debug_printf("- dg_hdio_init(%u)\n",channel_num);

    if (!dg_card_config[card_index].chan_is_initialized) {
        hdio_debug_printf("- dg_hdio_init init hdio_init_mutex\n");
        res_mutex = osl_init_uart_mutex(&dg_card_config[card_index].hdio_init_mutex);
        if (res_mutex!=DG_OK) return res_mutex;
    }
    res_mutex = osl_lock_uart_mutex(&dg_card_config[card_index].hdio_init_mutex);
    if (res_mutex!=DG_OK) return res_mutex;

    if (!dg_card_config[card_index].chan_is_initialized) {
        hdio_debug_printf("- dg_hdio_init init_channels\n");
        res = init_channels(card_index);
    }
    if (res==DG_OK && dg_card_config[card_index].dg_hdio_configs[channel_num].is_initialized == DG_HDIO_NOT_INITIALIZED) {
        dg_card_config[card_index].dg_hdio_configs[channel_num].is_initialized = DG_HDIO_IS_INITIALIZED;
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
            hdio_debug_printf("- dg_hdio_init parallel register IO layer\n");
            res = dg_hdreg_init(channel_num, card_index);
        }
#if (DG_IO_MODE==DG_HDCFG_SERIAL)
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_SERIAL ) {
            hdio_debug_printf("- dg_hdio_init serial stream IO layer\n");
            res = dg_hdstm_init(channel_num, card_index);
        }
#endif
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
            hdio_debug_printf("- dg_hdio_init usb register IO layer\n");
            res = dg_hdreg_init(channel_num, card_index);
        }
    }
    if ( res != DG_OK ) dg_card_config[card_index].chan_is_initialized = FALSE;
    hdio_debug_printf("- dg_hdio_init(%u)=%d\n",channel_num,res);

    res_mutex = osl_unlock_uart_mutex(&dg_card_config[card_index].hdio_init_mutex);
    if (res_mutex!=DG_OK) return res_mutex;

    /* init does not care if called multiple times */
    return res;
}

DG_RESULT
dg_hdio_transmit(
    DG_U8 channel_num,
    DG_U32 size,
    DG_U8* txBuff,
    const DG_S32 fn_time_out,
    const DG_S32 fn_start_time,
    DG_U8 card_index
)
{
    DG_RESULT res = DG_ERROR_HDIO_UNKNOWN_IO_MODE;
    DG_U32 curr_size = size;
    DG_S32 *function_time_out = LOCATE_CFG_FTO(card_index,channel_num);
    DG_S32 *function_start_time = LOCATE_CFG_FST(card_index,channel_num);

    *function_time_out = fn_time_out;
    *function_start_time = fn_start_time;
    hdio_debug_printf("- dg_hdio_transmit(%u,size = %u)\n",channel_num,size);
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        res=dg_hdreg_transmit(channel_num,&curr_size,txBuff, card_index);
    }
#if (DG_IO_MODE==DG_HDCFG_SERIAL)
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_SERIAL ) {
        res=dg_hdstm_transmit(channel_num,&curr_size,txBuff, card_index);
    }
#endif
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        res=dg_hdreg_transmit(channel_num,&curr_size,txBuff, card_index);
    }
    hdio_debug_printf("- dg_hdio_transmit()=res:%d,trans_size:%u\n",res,curr_size);
    return res;
}

DG_RESULT
dg_hdio_set_loopback(
    DG_U8 channel_num,
    DG_U8 card_index
)
{
    DG_RESULT res = DG_ERROR_HDIO_UNKNOWN_IO_MODE;
    hdio_debug_printf("- dg_hdio_set_loopback(%u)\n",channel_num);
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        res=dg_hdreg_set_loopback(channel_num, card_index);
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_SERIAL ) {
        res = DG_ERROR_HDIO_NOT_SUPPORTED;
        hdio_debug_printf("- No loopback on serial devices! res:%d\n",res);
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        res=dg_hdreg_set_loopback(channel_num, card_index);
    }
    hdio_debug_printf("- dg_hdio_set_loopback()=res:%d\n",res);
    return res;
}

DG_RESULT
dg_hdio_release_loopback(
    DG_U8 channel_num,
    DG_U8 card_index
)
{
    DG_RESULT res = DG_ERROR_HDIO_UNKNOWN_IO_MODE;
    hdio_debug_printf("- dg_hdio_release_loopback(%u)\n",channel_num);

    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        res=dg_hdreg_release_loopback(channel_num, card_index);
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_SERIAL ) {
        res = DG_ERROR_HDIO_NOT_SUPPORTED;
        hdio_debug_printf("- No loopback on serial devices!\n res:%d ",res);
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        res=dg_hdreg_release_loopback(channel_num, card_index);
    }
    hdio_debug_printf("- dg_hdio_release_loopback()=res:%d\n",res);
    return res;
}

DG_DECL_PUBLIC
DG_RESULT
dg_hdio_receive(
    DG_U8 channel_num,
    DG_U32 size,
    DG_U8* rxBuff,
    DG_U8 card_index
)
{
    DG_RESULT res = DG_ERROR_HDIO_UNKNOWN_IO_MODE;
    DG_U32 curr_size = size;
    hdio_debug_printf("- dg_hdio_receive(%u,size = %u)\n",channel_num,size);
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        res=dg_hdreg_receive(channel_num,&curr_size,rxBuff, card_index);
    }
#if (DG_IO_MODE==DG_HDCFG_SERIAL)
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_SERIAL ) {
        res=dg_hdstm_receive(channel_num,&curr_size,rxBuff, card_index);
    }
#endif

    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        res=dg_hdreg_receive(channel_num,&curr_size,rxBuff, card_index);
    }
    hdio_debug_printf("- dg_hdio_receive()=res:%d,trans_size:%u\n",res,curr_size);
    return res;
}

DG_DECL_PUBLIC
DG_BOOL
dg_hdio_is_polling_channel(
        DG_U8 channel_num,
        DG_U8 card_index
)
{
    hdio_debug_printf("%d %s\n", channel_num, dg_card_config[card_index].dg_hdio_configs[channel_num].is_polling?"polling":"normal");
    return  dg_card_config[card_index].dg_hdio_configs[channel_num].is_polling;
}


DG_DECL_PUBLIC
DG_RESULT
dg_hdio_wait_to_send(
   DG_U8 channel_num
)
{
    (void)channel_num;
    return DG_OK; /* hdio always ready to send */
}

DG_RESULT
dg_hdio_sanity_check(
    DG_U8 channel_num,
    DG_U8 card_index
)
{
    DG_RESULT res = DG_ERROR_HDIO_UNKNOWN_IO_MODE;
    hdio_debug_printf("- dg_hdio_sanity_check(%u)\n",channel_num);
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        res=dg_hdreg_sanity_check(channel_num, card_index);
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_SERIAL ) {
        res = DG_OK;
        hdio_debug_printf("- No sanity check on serial devices!\n res:%d" ,res);
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
        res=dg_hdreg_sanity_check(channel_num, card_index);
    }
    hdio_debug_printf("- dg_hdio_sanity_check()=res:%d\n",res);
    return res;
}
