/* ==========================================================================
 *
 *  File      : DG_HDREG.C
 *
 *  Purpose   : Duagon Host Device IO -
 *              Register access implementation
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
#include "dg_reg2g.h"
#include "dg_reg3g.h"
#if (DG_IO_MODE==DG_HDCFG_USB)
#include "dg_hdusb.h"
#endif
#include "dg_hdpar.h"


#if (1 == HDIO_DEBUG)
#define hdreg_debug_printf osl_printf
#else
#define hdreg_debug_printf dummy_printf
/* dummy printf to avoid warnings */
DG_DECL_LOCAL void dummy_printf(DG_DECL_CONST DG_CHAR8* nothing,...) {(void)nothing;}
#endif

/* ==========================================================================
 *
 *  Public Procedures (general)
 *
 * ==========================================================================
 */
DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_transmit
(
        DG_U8 channel_num,
        DG_U32 *curr_size,
        DG_U8* txBuff,
        DG_U8 card_index
)
{
    hdreg_debug_printf("dg_hdreg_transmit\n");
    switch (dg_card_config[card_index].dg_hdio_configs[channel_num].channel_type) {
        case DG_HDCFG_CHANNEL_2G:
            return dg_reg_transmit_2g(channel_num, curr_size, txBuff, card_index);
        break;
        case DG_HDCFG_CHANNEL_1G:
            return dg_reg_transmit_1g(channel_num, curr_size, txBuff, card_index);
        break;
         case DG_HDCFG_CHANNEL_3G:
            return dg_reg_transmit_3g(channel_num, curr_size, txBuff, card_index);
        break;
        default:
        break;
    }
    return DG_ERROR_HDIO_UNKNOWN_CHANNEL_TYPE;
}

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_receive
(
        DG_U8 channel_num,
        DG_U32 *curr_size,
        DG_U8* rxBuff,
        DG_U8 card_index
)
{
    hdreg_debug_printf("dg_hdreg_receive\n");
    switch (dg_card_config[card_index].dg_hdio_configs[channel_num].channel_type) {
        case DG_HDCFG_CHANNEL_2G:
            return dg_reg_receive_2g(channel_num, curr_size, rxBuff, card_index);
        break;
        case DG_HDCFG_CHANNEL_1G:
            return dg_reg_receive_1g(channel_num, curr_size, rxBuff, card_index);
        break;
        case DG_HDCFG_CHANNEL_3G:
            return dg_reg_receive_3g(channel_num, curr_size, rxBuff, card_index);
        break;
        default:
        break;
    }
    return DG_ERROR_HDIO_UNKNOWN_CHANNEL_TYPE;
}

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_set_loopback
(
        DG_U8 channel_num,
        DG_U8 card_index
)
{
    DG_RESULT          result = DG_ERROR_HDIO_UNKNOWN_CHANNEL_TYPE;
    if (DG_HDCFG_CHANNEL_2G == dg_card_config[card_index].dg_hdio_configs[channel_num].channel_type) {
        result = dg_reg_set_loopback_2g(channel_num, card_index);
    }
    return result;
}

DG_DECL_LOCAL
DG_BOOL
is_card_mdfull_type
(
    DG_U8 card_index
)
{
    DG_U8 card_number;
    DG_BOOL mdfull = FALSE;
    
    card_number = dg_card_config[card_index].card_number;
        
    switch ( card_number )
    {
        case 0:
            if ( CARD_0_TYPE==CARD_MVB)
            {
                mdfull = TRUE;
            }
            break;
        case 1:
            if ( CARD_1_TYPE==CARD_MVB)
            {
                mdfull = TRUE;
            }
            break;
        case 2:
            if ( CARD_2_TYPE==CARD_MVB)
            {
                mdfull = TRUE;
            }
            break;
        case 3:
            if ( CARD_3_TYPE==CARD_MVB)
            {
                mdfull = TRUE;
            }
            break;
        default:
            break;
    }
    

    return mdfull;
}
DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_release_loopback
(
        DG_U8 channel_num,
        DG_U8 card_index
)
{
    DG_RESULT          result = DG_ERROR_HDIO_UNKNOWN_CHANNEL_TYPE;
    if (DG_HDCFG_CHANNEL_2G == dg_card_config[card_index].dg_hdio_configs[channel_num].channel_type) {
        result = dg_reg_release_loopback_2g(channel_num, card_index);
    }
    return result;
}

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_init
(
        DG_U8 channel_num,
        DG_U8 card_index
)
{
    DG_RESULT ret_val = DG_OK;
    DG_BOOL is_mdfull_card = FALSE;
    hdreg_debug_printf("dg_hdreg_init\n");


    hdreg_debug_printf ( "dg_hdio_configs channel_type %X\n", dg_card_config[card_index].dg_hdio_configs[channel_num].channel_type );
    switch ( dg_card_config[card_index].dg_hdio_configs[channel_num].channel_type )
    {
    case DG_HDCFG_CHANNEL_2G:
#ifdef D017TYPE 
        if(channel_num == 2)
        {
            is_mdfull_card = is_card_mdfull_type(card_index);
            if(is_mdfull_card == FALSE)
            {
                ret_val = dg_reg_do_init_sanity_check_2g(channel_num, card_index);  
            }
        }
        else
        {
            ret_val = dg_reg_do_init_sanity_check_2g(channel_num, card_index);  
        }
#else
        ret_val = dg_reg_do_init_sanity_check_2g(channel_num, card_index);  
#endif
        if ( ret_val != DG_OK) return ret_val;
        break;
    case DG_HDCFG_CHANNEL_1G:
    case DG_HDCFG_CHANNEL_3G:
    default:
        break;
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        hdreg_debug_printf("init channel %d as parallel\n", channel_num);
        return osl_par_hal_init_channel(
            (DG_HDREG_CONFIG*)&dg_card_config[card_index].dg_hdio_configs[channel_num]);
    }
#if (DG_IO_MODE==DG_HDCFG_USB)
    else {
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
            hdreg_debug_printf("init channel %d as usb\n", channel_num);
            return osl_usb_hal_init_channel(&dg_card_config[card_index].dg_hdio_configs[channel_num], card_index);
        }
    }
#endif
    return DG_ERROR_HDIO_UNKNOWN_IO_MODE;
}

/* ATTENTION */
/* PCI Scan works only with D221 not yet with D213 */
DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_install
(
        DG_U8 card_index
)
{
    DG_U32 i;
    DG_U32 addr_list[DG_NR_OF_CHANNELS];
    DG_RESULT res = DG_ERROR_HDIO_UNKNOWN_IO_MODE;
    DG_HDREG_CONFIG *config  = NULL;

    hdreg_debug_printf("dg_hdreg_install\n");
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
        /* PCI Scan not possible without giveio - start it first*/
        /* initializes the par driver (opens giveio) */
        res = osl_par_hal_install(addr_list, DG_NR_OF_CHANNELS, card_index);

        /* Build channel info structure */
        for (i=0;i<DG_NR_OF_CHANNELS;i++) {
            hdreg_debug_printf("setting config %d\n", i);
            switch (dg_card_config[card_index].dg_hdio_configs[i].channel_type) {
                /* It's a UE2G channel */
                case DG_HDCFG_CHANNEL_2G:
                    hdreg_debug_printf("setting config 2g\n");
                    config = (DG_HDREG_CONFIG*)&dg_card_config[card_index].dg_hdio_configs[i];
                    /* Store the base address of the channel into the structure */
                    config->base = (UE2G_REGISTER_MAP*)addr_list[i];
                break;
                /* It's a UE1G channel */
                case DG_HDCFG_CHANNEL_1G:
                    hdreg_debug_printf("setting config 1g\n");
                    config = (DG_HDREG_CONFIG*)&dg_card_config[card_index].dg_hdio_configs[i];
                    /* Store the base address of the channel into the structure */
                    config->base = (UE1G_REGISTER_MAP*)addr_list[i];
                break;
                /* It's a UE3G channel */
                case DG_HDCFG_CHANNEL_3G:
                    hdreg_debug_printf("setting config 3g\n");
                    config = (DG_HDREG_CONFIG*)&dg_card_config[card_index].dg_hdio_configs[i];
                    /* Store the base address of the channel into the structure */
                    config->base = (UE3G_REGISTER_MAP*)addr_list[i];
                break;
                default:
                break;
            }
            hdreg_debug_printf("config_base: 0x%08X\n",(DG_U32)config->base);

        }
    }
#if (DG_IO_MODE==DG_HDCFG_USB)
    else {
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB )
        /* initializes the usb driver (opens the usb device) */
        res = osl_usb_hal_install(card_index);

        /* status read to get the usb connection to live.
         * this read will answer with a timeout (which slows down the
         * initialization of the usb driver, but allows it to work
         */
        for (i=0;i<DG_NR_OF_CHANNELS;i++) {
            if (DG_HDCFG_CHANNEL_2G == dg_card_config[card_index].dg_hdio_configs[i].channel_type) {
                config = (DG_HDREG_CONFIG*)&dg_card_config[card_index].dg_hdio_configs[i];
                hd_reg_get_status(&(((REG_SIZE*)config->base)[4]), card_index);
            }
        }
    }
#endif
    return res;
}


DG_DECL_PUBLIC
DG_U8
hd_reg_get_status
(
        REG_SIZE* addr,
        DG_U8 card_index
)
{
    DG_RESULT res = DG_ERROR_HDIO_UNKNOWN_IO_MODE;
    DG_U32 size = 1;
    DG_U8 space = 0;
    
    hdreg_debug_printf("dg_hdreg_get_status 0x%08X\n", (DG_U32)addr);
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_USB ) {
#if (DG_IO_MODE==DG_HDCFG_USB)
        res = osl_usb_hal_read(addr, &size, &space, card_index);
        hdreg_debug_printf("dg_hdreg_get_status res=%d\n", res);
        if (DG_OK == res) {
            hdreg_debug_printf("dg_hdreg_get_status space=%d\n", space);
            return space;
        }
#else
        (void)res;
        (void)size;
#endif
    }
    else {
        if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL ) {
            space = osl_par_hal_read8(addr);
            if (space != 0)
                hdreg_debug_printf("dg_hdreg_get_status space=%d\n", space);
            
            return space;
        }
    }
    return 0;
}

DG_DECL_PUBLIC
DG_RESULT
dg_hdreg_sanity_check
(
        DG_U8 channel_num,
        DG_U8 card_index
)
{
    DG_RESULT          result = DG_ERROR_HDIO_UNKNOWN_CHANNEL_TYPE;
    if (DG_HDCFG_CHANNEL_2G == dg_card_config[card_index].dg_hdio_configs[channel_num].channel_type) {
        result = dg_reg_do_empty_full_sanity_check_2g(channel_num, card_index);
    }
    return result;
}
