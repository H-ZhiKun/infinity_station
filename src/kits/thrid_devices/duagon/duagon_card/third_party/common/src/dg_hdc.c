/* ==========================================================================
 *
 *  File      : DG_HDC.C
 *
 *  Purpose   : Host Device Communication Layer between RPC and HDIO
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

#include "dg_hdc.h"
#include "dg_hdio.h"
#include "dg_rtm.h"
#include "osl.h"

#include "mdfl_bc.h"
#include "mdfl_ls.h"
#include "mdfl_lp.h"
#include "mdfl_lm.h"


#ifdef PROT_CO
#include "dg_co.h"
#endif
#ifdef PROT_SOCKETS
#include "dg_ip.h"
#endif
#ifdef PROT_PFS
#include "dg_pfs.h"
#endif
#ifdef WEBMAINTENANCE
#include "dg_wm.h"
#endif
#ifdef PROT_IPTCOM_PD
#include "dg_iptpd.h"
#endif
#ifdef PROT_IPTCOM_MD
#include "dg_iptmd.h"
#endif
#ifdef PROT_IPTCOM_TDC
#include "dg_ipttdc.h"
#endif
#ifdef DG_SVC_CHANNEL
#include "dg_svc.h"
#endif
#ifdef PROT_CIP
#include "dg_cip.h"
#endif
#ifdef TESTBOX_MASTER
#include "dg_prod_test_s.h"
#endif
#ifdef PROT_MVB_MD_FULL
#include "dg_perf.h" /* MVB_PROTOCOL_ID */
#endif

#ifdef PROT_TRDP
#include "dg_trdp.h"
#endif
#include <stdio.h>

#if (1 == HDC_DEBUG)
#define hdc_debug_printf osl_printf
#else
#define hdc_debug_printf dummy_printf
DG_DECL_LOCAL void dummy_printf(DG_DECL_CONST DG_CHAR8* nothing,...) {(void)nothing; } /* dummy printf to avoid warnings */
#endif




/*
 * ====================================================
 *
 * table allowing function specific time-outs
 * - can be populated if required (see typical entry)
 * ====================================================
 */

typedef struct {
    DG_U8 protocol;
    DG_U8 command;
    DG_U32 time_out;
} HDC_FUNCTION_TIMEOUTS;

#define LAST_ENTRY_MARKER 0
HDC_FUNCTION_TIMEOUTS time_outs_tab[] = {
/*  {PFS_PROTOCOL_ID, PFS_POSIX_OPEN, POSIX_OPEN_MAX_DELAY_MICRO_SEC}, //typical entry */
    { LAST_ENTRY_MARKER, 0, 0 } /* do not delete */
};

/* ------------------------------------------------------ */

typedef struct {
    DG_U8 protocol;
    DG_U8 command; /* in case of protocol HDC_PROTOCOL_ASKWAIT: handle */
    DG_U16 payloadLen;
} HEADER;



DG_DECL_PUBLIC
DG_U8
dg_get_channel_for_protocol(DG_U8 protocol)
{
    switch (protocol)
    {
#ifdef CANOPEN_PROTOCOL_ID
    case CANOPEN_PROTOCOL_ID: /* protocol number for rpc based CANopen implementation */
    return DG_CAN_CHANNEL;
#endif
#ifdef SOCKET_PROTOCOL_ID
    case SOCKET_PROTOCOL_ID: /* protocol number for rpc based socket implementation */
    return DG_SOCKETS_CHANNEL;
#endif
#ifdef PFS_PROTOCOL_ID
    case PFS_PROTOCOL_ID: /* protocol number for rpc based posix implementation */
    return DG_PFS_CHANNEL;
#endif
#ifdef IPTCOM_PD_PROTOCOL_ID
    case IPTCOM_PD_PROTOCOL_ID: /* protocol number for rpc based iptcom pd implementation */
    return DG_IPT_PD_CHANNEL;
#endif
#ifdef IPTCOM_MD_PROTOCOL_ID
    case IPTCOM_MD_PROTOCOL_ID: /* protocol number for rpc based iptcom md implementation */
    return DG_IPT_MD_CHANNEL;
#endif
#ifdef IPTCOM_TDC_PROTOCOL_ID
	case IPTCOM_TDC_PROTOCOL_ID: /* protocol number for rpc based iptcom md implementation */
	return DG_IPT_TDC_CHANNEL;
#endif
#ifdef SVC_PROTOCOL_ID
    case SVC_PROTOCOL_ID: /* protocol number for rpc based service implementation */
    return DG_SVC_CHANNEL;
#endif
#ifdef CIP_PROTOCOL_ID
    case CIP_PROTOCOL_ID: /* protocol number for rpc based CIP implementation */
    return DG_CIP_CHANNEL;
#endif
#ifdef WM_PROTOCOL_ID
	case WM_PROTOCOL_ID: /* workaround for MVB timing measurement*/
	return DG_WM_CHANNEL;
#endif
#ifdef TRDP_PD_PROTOCOL_ID
	case TRDP_PD_PROTOCOL_ID:
    return DG_PD_TRDP_CHANNEL;
    case TRDP_MD_PROTOCOL_ID:
    return DG_MD_TRDP_CHANNEL;
#endif

#ifdef PROD_TEST_HDC_PROTOCOL_NUM
    case PROD_TEST_HDC_PROTOCOL_NUM: /* protocol number for rpc based service implementation -- testbox only*/
    return PROD_TEST_CHANNEL;
#endif
#ifdef MVB_PROTOCOL_ID
    case MVB_PROTOCOL_ID: /* workaround for MVB timing measurement*/
    return MVB_PROTOCOL_CHANNEL;
#endif
    default:
        return 0xff; /* definitely wrong */
    }
}


void dg_hdc_set_perf_callbacks(DuagonBoundaryFunc start, DuagonBoundaryFunc send, DuagonBoundaryFunc recv_h, DuagonBoundaryFunc recv_p, DG_U8 card_index) {
    dg_card_config[card_index].hdc.start_func = start;
    dg_card_config[card_index].hdc.send_func = send;
    dg_card_config[card_index].hdc.recv_h_func = recv_h;
    dg_card_config[card_index].hdc.recv_p_func = recv_p;
}


DG_RESULT
hdc_init_card(DG_U8 card_index, DG_U8 card_number)
{
    DG_RESULT result = DG_OK;

    dg_card_config[card_index].ip.ip_mutex_init_done=FALSE;
    dg_card_config[card_index].ic.ipt_md_mutex_init_done=FALSE;
    dg_card_config[card_index].msch.clch_return_number=0;
    dg_card_config[card_index].hdc.recv_h_func=NULL;
    dg_card_config[card_index].hdc.recv_p_func=NULL;
    dg_card_config[card_index].hdc.send_func=NULL;
    dg_card_config[card_index].hdc.start_func=NULL;
    dg_card_config[card_index].msv.in_func=NULL;
    dg_card_config[card_index].msv.out_func=NULL;
    dg_card_config[card_index].mscp.recv_h_func=NULL;
    dg_card_config[card_index].mscp.recv_p_func=NULL;
    dg_card_config[card_index].mscp.send_func=NULL;
    dg_card_config[card_index].mscp.start_func=NULL;
    dg_card_config[card_index].mupd.recv_h_func=NULL;
    dg_card_config[card_index].mupd.recv_p_func=NULL;
    dg_card_config[card_index].mupd.send_func=NULL;
    dg_card_config[card_index].mupd.start_func=NULL;

    dg_card_config[card_index].mscf.bCgfCallbackBusy=FALSE;
    dg_card_config[card_index].mscf.bCgfSupervisor=FALSE;
    dg_card_config[card_index].mscf.cgf_device_status_supervisor=GF_DEVICE_STATUS_OK;
    dg_card_config[card_index].mscf.cgf_device_status_old=GF_DEVICE_STATUS_OK;
    dg_card_config[card_index].mscf.cgf_device_status=GF_DEVICE_STATUS_OK;
    dg_card_config[card_index].mscf.cgf_server_status=0;
    dg_card_config[card_index].mscf.bDone_gf_open_device=FALSE;
    dg_card_config[card_index].mscf.bDone_gf_init_device=FALSE;
    dg_card_config[card_index].mscf.bDone_gf_close_device=FALSE;
    dg_card_config[card_index].mscf.bDone_gf_mvb_init=FALSE;
    dg_card_config[card_index].mscf.bDone_gf_mvb_start=FALSE;
    dg_card_config[card_index].mscf.bDone_lp_init=FALSE;
    dg_card_config[card_index].mscf.bDone_am_init=FALSE;
    dg_card_config[card_index].mscf.bDone_am_announce_device=FALSE;
    dg_card_config[card_index].mscf.bDone_dsw_init=FALSE;
    dg_card_config[card_index].mscf.bDone_ba_init=FALSE;

    dg_card_config[card_index].card_number=card_number;
    dg_card_config[card_index].is_initialized=DG_HDIO_NOT_INITIALIZED;
    dg_card_config[card_index].hdc_init_done=FALSE;
    dg_card_config[card_index].chan_is_initialized=FALSE;
    dg_card_config[card_index].rpc.send_func = (DuagonSendFunc)dg_hdc_request;
    dg_card_config[card_index].rpc.in_func = NULL;
    dg_card_config[card_index].rpc.out_func = NULL;

    dg_card_config[card_index].tcbs.bc_mdfl.acc_reg_rbr_thr=0;
    dg_card_config[card_index].tcbs.bc_mdfl.acc_reg_lsr=0;
    dg_card_config[card_index].tcbs.bc_mdfl.acc_reg_size=0;
    dg_card_config[card_index].tcbs.bc_mdfl.acc_reg_timeout=0;
    dg_card_config[card_index].tcbs.bc_mdfl.ls_ds_capability=TRUE;
    dg_card_config[card_index].tcbs.bc_mdfl.ls_ba_capability=TRUE;
    dg_card_config[card_index].tcbs.bc_mdfl.ls_ba_list_entries=8192;

    dg_card_config[card_index].tcbs.bls_desc_0.p_bus_ctrl=
            &dg_card_config[card_index].tcbs.bc_mdfl;
    dg_card_config[card_index].tcbs.bls_desc_0.link_type=1; /* SV_LINK_TYPE_MVB */
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[0]= 'D'; /*{ "Duagon MVB  MDFULL" };*/
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[1]= 'u';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[2]= 'a';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[3]= 'g';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[4]= 'o';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[5]= 'n';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[6]= ' ';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[7]= 'M';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[8]= 'V';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[9]= 'B';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[10]= ' ';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[11]= ' ';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[12]= 'M';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[13]= 'D';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[14]= 'F';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[15]= 'U';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[16]= 'L';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[17]= 'L';
    dg_card_config[card_index].tcbs.bls_desc_0.link_name.character[18]= '\0';
#ifdef PROT_MVB_MD_FULL
    dg_card_config[card_index].tcbs.bls_desc_0.desc_sv.ls_init=(DG_LS_INIT)mdfull_ls_init_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_sv.ls_service_handler=(DG_LS_SERVICE_HANDLER)mdfull_ls_service_handler_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_pd.lp_init=(DG_LP_INIT)mdfull_lp_init_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_pd.lp_manage=(DG_LP_MANAGE)mdfull_lp_manage_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_pd.lp_put_dataset=(DG_LP_PUT_DATASET)mdfull_lp_put_dataset_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_pd.lp_get_dataset=(DG_LP_GET_DATASET)mdfull_lp_get_dataset_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_init=(DG_LM_INIT)mdfull_lm_init_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_get_dev_address=(DG_LM_GET_DEV_ADDRESS)mdfull_lm_get_dev_address_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_get_status=(DG_LM_GET_STATUS)mdfull_lm_get_status_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_send_queue_flush=(DG_LM_SEND_QUEUE_FLUSH)mdfull_lm_send_queue_flush_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_send_request=(DG_LM_SEND_REQUEST)mdfull_lm_send_request_idx;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_receive_poll=(DG_LM_RECEIVE_POLL)mdfull_lm_receive_poll_idx;
#endif
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_receive_indicate=NULL;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_get_pack=NULL;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_packet_pool=NULL;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_send_confirm=NULL;
    dg_card_config[card_index].tcbs.bls_desc_0.desc_md.lm_status_indicate=NULL;
    dg_card_config[card_index].tcap.p_tcn_ap_port_buffer = dg_card_config[card_index].tcap.tcn_ap_port_buffer;
    dg_card_config[card_index].p1g.write_func = NULL;
    dg_card_config[card_index].p1g.read_d_func = NULL;
    dg_card_config[card_index].p1g.read_s_func = NULL;
    dg_card_config[card_index].p1g.is_polling_func = NULL;
    dg_card_config[card_index].p3g.write_func = NULL;
    dg_card_config[card_index].p3g.read_d_func = NULL;
    dg_card_config[card_index].p3g.read_s_func = NULL;
    dg_card_config[card_index].p3g.is_polling_func = NULL;
    dg_card_config[card_index].p2g.write_func = NULL;
    dg_card_config[card_index].p2g.read_d_func = NULL;
    dg_card_config[card_index].p2g.read_s_func = NULL;
    dg_card_config[card_index].p2g.is_polling_func = NULL;
    {
        DG_U8 i;
        for ( i=1; i<DG_BLS_MAX_LINKS; i++)
        {
            dg_card_config[card_index].tcbs.bls_descr_table[i] = NULL;
        }
    }
    dg_card_config[card_index].tcbs.bls_descr_table[0] = (void *)&dg_card_config[card_index].tcbs.bls_desc_0;

#if DG_IO_MODE == DG_HDCFG_PARALLEL

    switch ( card_number )
    {
        case 0:
#           if defined CARD_0_HOST_BASE_CHANNEL_0
                dg_card_config[card_index].dg_io_mode=DG_IO_MODE_ALL;
                dg_card_config[card_index].host_base_channel[0]=CARD_0_HOST_BASE_CHANNEL_0;
#               if ( DG_NR_OF_CHANNELS > 1 )
                    dg_card_config[card_index].host_base_channel[1]=CARD_0_HOST_BASE_CHANNEL_1;
#               endif
#               if ( DG_NR_OF_CHANNELS > 2 )
                    dg_card_config[card_index].host_base_channel[2]=CARD_0_HOST_BASE_CHANNEL_2;
#               endif
#               if ( DG_NR_OF_CHANNELS > 3 )
                    dg_card_config[card_index].host_base_channel[3]=CARD_0_HOST_BASE_CHANNEL_3;
#               endif
#               if ( CARD_0_TYPE==CARD_MVB )
                    dg_card_config[card_index].mvb_channel=DG_CARD_0_MVB_CHANNEL;
#               endif                    
#               if ( CARD_0_TYPE==SERVER2 )
                    dg_card_config[card_index].mvb_channel=DG_CARD_0_MVB_CHANNEL;
                    dg_card_config[card_index].mvb_srv_channel=DG_CARD_0_MVB_SERVER_CHANNEL;
#               endif
#           endif
            break;
        case 1:
#           if defined CARD_1_HOST_BASE_CHANNEL_0
                dg_card_config[card_index].dg_io_mode=DG_IO_MODE_ALL;
                dg_card_config[card_index].host_base_channel[0]=CARD_1_HOST_BASE_CHANNEL_0;
#               if ( DG_NR_OF_CHANNELS > 1 )
                    dg_card_config[card_index].host_base_channel[1]=CARD_1_HOST_BASE_CHANNEL_1;
#               endif
#               if ( DG_NR_OF_CHANNELS > 2 )
                    dg_card_config[card_index].host_base_channel[2]=CARD_1_HOST_BASE_CHANNEL_2;
#               endif
#               if ( DG_NR_OF_CHANNELS > 3 )
                    dg_card_config[card_index].host_base_channel[3]=CARD_1_HOST_BASE_CHANNEL_3;
#               endif
#               if ( CARD_1_TYPE==CARD_MVB )
                    dg_card_config[card_index].mvb_channel=DG_CARD_1_MVB_CHANNEL;
#               endif                    
#               if ( CARD_1_TYPE==SERVER2 )
                    dg_card_config[card_index].mvb_channel=DG_CARD_1_MVB_CHANNEL;
                    dg_card_config[card_index].mvb_srv_channel=DG_CARD_1_MVB_SERVER_CHANNEL;
#               endif
#           endif
            break;
        case 2:
#           if defined CARD_2_HOST_BASE_CHANNEL_0
                dg_card_config[card_index].dg_io_mode=DG_IO_MODE_ALL;
                dg_card_config[card_index].host_base_channel[0]=CARD_2_HOST_BASE_CHANNEL_0;
#               if ( DG_NR_OF_CHANNELS > 1 )
                dg_card_config[card_index].host_base_channel[1]=CARD_2_HOST_BASE_CHANNEL_1;
#               endif
#               if ( DG_NR_OF_CHANNELS > 2 )
                dg_card_config[card_index].host_base_channel[2]=CARD_2_HOST_BASE_CHANNEL_2;
#               endif
#               if ( DG_NR_OF_CHANNELS > 3 )
                dg_card_config[card_index].host_base_channel[3]=CARD_2_HOST_BASE_CHANNEL_3;
#               endif
#               if ( CARD_2_TYPE==CARD_MVB )
                dg_card_config[card_index].mvb_channel=DG_CARD_2_MVB_CHANNEL;
#               endif
#               if ( CARD_2_TYPE==SERVER2 )
                    dg_card_config[card_index].mvb_channel=DG_CARD_2_MVB_CHANNEL;
                    dg_card_config[card_index].mvb_srv_channel=DG_CARD_2_MVB_SERVER_CHANNEL;
#               endif
#           endif
            break;
        case 3:
#           if defined CARD_3_HOST_BASE_CHANNEL_0
                dg_card_config[card_index].dg_io_mode=DG_IO_MODE_ALL;
                dg_card_config[card_index].host_base_channel[0]=CARD_3_HOST_BASE_CHANNEL_0;
#               if ( DG_NR_OF_CHANNELS > 1 )
                dg_card_config[card_index].host_base_channel[1]=CARD_3_HOST_BASE_CHANNEL_1;
#               endif
#               if ( DG_NR_OF_CHANNELS > 2 )
                dg_card_config[card_index].host_base_channel[2]=CARD_3_HOST_BASE_CHANNEL_2;
#               endif
#               if ( DG_NR_OF_CHANNELS > 3 )
                dg_card_config[card_index].host_base_channel[3]=CARD_3_HOST_BASE_CHANNEL_3;
#               endif
#               if ( CARD_3_TYPE==CARD_MVB )
                dg_card_config[card_index].mvb_channel=DG_CARD_3_MVB_CHANNEL;
#               endif
#               if ( CARD_3_TYPE==SERVER2 )
                    dg_card_config[card_index].mvb_channel=DG_CARD_3_MVB_CHANNEL;
                    dg_card_config[card_index].mvb_srv_channel=DG_CARD_3_MVB_SERVER_CHANNEL;
#               endif
#           endif
            break;
        default:
            result = DG_ERROR;
            break;
    }
#elif  DG_IO_MODE == DG_HDCFG_USB
    dg_card_config[card_index].dg_io_mode=DG_HDCFG_USB;
    dg_card_config[card_index].host_base_channel[0]=0x80;
#   if ( DG_NR_OF_CHANNELS > 1 )
        dg_card_config[card_index].host_base_channel[1]=0x88;
#   endif
#   if ( DG_NR_OF_CHANNELS > 2 )
        dg_card_config[card_index].host_base_channel[2]=0x40;
#   endif
#   if ( DG_NR_OF_CHANNELS > 3 )
        dg_card_config[card_index].host_base_channel[3]=0x48;
#   endif

    switch ( card_number )
    {
        case 0:
#         if ( CARD_0_TYPE==CARD_MVB )
             dg_card_config[card_index].mvb_channel=DG_CARD_0_MVB_CHANNEL;
#         endif
          break;
        case 1:
#         if ( CARD_1_TYPE==CARD_MVB )
             dg_card_config[card_index].mvb_channel=DG_CARD_1_MVB_CHANNEL;
#         endif
          break;
        case 2:
#         if ( CARD_2_TYPE==CARD_MVB )
             dg_card_config[card_index].mvb_channel=DG_CARD_2_MVB_CHANNEL;
#         endif
          break;
        case 3:
#         if ( CARD_3_TYPE==CARD_MVB )
             dg_card_config[card_index].mvb_channel=DG_CARD_3_MVB_CHANNEL;
#         endif
          break;
        default:
            result = DG_ERROR;
            break;
    }
#elif  DG_IO_MODE == DG_HDCFG_SERIAL
    dg_card_config[card_index].dg_io_mode=DG_HDCFG_SERIAL;
    dg_card_config[card_index].host_base_channel[0]=0x0;
#   if ( DG_NR_OF_CHANNELS > 1 )
        dg_card_config[card_index].host_base_channel[1]=0x1;
#   endif
#   if ( DG_NR_OF_CHANNELS > 2 )
        dg_card_config[card_index].host_base_channel[2]=0x2;
#   endif
#   if ( DG_NR_OF_CHANNELS > 3 )
        dg_card_config[card_index].host_base_channel[3]=0x3;
#   endif

    switch ( card_number )
    {
        case 0:
#         if ( CARD_0_TYPE==CARD_MVB )
             dg_card_config[card_index].mvb_channel=DG_CARD_0_MVB_CHANNEL;
#         endif
          break;
        case 1:
#         if ( CARD_1_TYPE==CARD_MVB )
             dg_card_config[card_index].mvb_channel=DG_CARD_1_MVB_CHANNEL;
#         endif
          break;
        case 2:
#         if ( CARD_2_TYPE==CARD_MVB )
             dg_card_config[card_index].mvb_channel=DG_CARD_2_MVB_CHANNEL;
#         endif
          break;
        case 3:
#         if ( CARD_3_TYPE==CARD_MVB )
             dg_card_config[card_index].mvb_channel=DG_CARD_3_MVB_CHANNEL;
#         endif
          break;
        default:
            result = DG_ERROR;
            break;
    }
#endif /* DG_IO_MODE */
    return result;
}

DG_RESULT
hdc_init_channels(DG_U8 card_index)
{
    DG_U8 i;
    DG_RESULT result = DG_OK;

    for (i=0; i<DG_NR_OF_CHANNELS && result==DG_OK; i++)
    {
        dg_card_config[card_index].channel_timeout[i] = FALSE;
        result = dg_hdio_init(i,card_index);
        if (result==DG_OK) {
            dg_card_config[card_index].is_initialized = DG_HDIO_IS_INITIALIZED;
            result = osl_init_uart_mutex(&dg_card_config[card_index].channel_mutex[i]);
        }
    }
    return result;
}


DLL 
DG_DECL_PUBLIC
DG_RESULT
hdc_init(DG_U8 card_index)
{
    DG_U8  cd_nbr;
    DG_RESULT result=DG_OK;
    DG_U8 card_number=DG_NO_CARD;

    for ( cd_nbr=0; cd_nbr<DG_MAX_CARDS_PER_DRIVER; cd_nbr++)
    {
       if ( card_defined[cd_nbr].defined && card_defined[cd_nbr].index == card_index )
           card_number = cd_nbr;
    }
    if (card_number != DG_NO_CARD)
    {
        if (!card_defined[card_number].card_init_done)
        {
           hdc_init_card(card_index, card_number);
        }
        card_defined[card_number].card_init_done=TRUE;
        if ( dg_card_config[card_index].hdc_init_done == FALSE )
        {
            result = hdc_init_channels(card_index);
        }
        if ( DG_OK == result )
        {
            dg_card_config[card_index].hdc_init_done=TRUE;
        }
    }
    else
    {
       result = DG_ERROR_CARD_NOT_FOUND;
    }
    return result;
}


RPC_DECL_LOCAL OSL_MUTEX*
dg_get_mutex_for_channel(DG_U8 card_index, DG_U8 channel){
    return &dg_card_config[card_index].channel_mutex[channel];
}

RPC_DECL_LOCAL DG_U16
dg_calculate_payload_length (DG_U32 start, DG_U32 nb_entries, DG_U32 nb_bytes[])
{
    DG_U16 payload_length = 0;
    DG_U32 i;
    DG_U32 mod4;

    for (i = start; i < nb_entries ; i++)
    {
        mod4 = nb_bytes[i] % 4;
        if (mod4>0)
            payload_length = (DG_U16)(payload_length + (nb_bytes[i] + (4-mod4))/4);
        else
            payload_length = (DG_U16)(payload_length + (nb_bytes[i]/4));
    }
    return payload_length;
}

RPC_DECL_LOCAL DG_RESULT dg_hdc_send_header(
        const DG_U8 protocol,
        const HEADER* header,
        const DG_S32 fn_time_out,
        const DG_S32 fn_start_time,
        const DG_U8 card_index
        )
{
    DG_RESULT result;
    DG_U8 headerStream[4];

    headerStream[0] = header->protocol;
    headerStream[1] = header->command;
    headerStream[2] = (DG_U8)(header->payloadLen & 0xFF);
    headerStream[3] = (DG_U8)(header->payloadLen >> 8);

    if ((result = dg_hdio_transmit(dg_get_channel_for_protocol(protocol), 4, headerStream, fn_time_out, fn_start_time, card_index)) != DG_OK)   {
        hdc_debug_printf("[%d] hdio transmit error (%d) when sending protocol number\n",dg_get_channel_for_protocol(protocol), result);
        return result;
    }
    return DG_OK;
}

RPC_DECL_LOCAL DG_RESULT dg_hdc_send_payload (
        const DG_U8 protocol,
        const RPC_PROTOCOL_DATA *send,
        const DG_S32 fn_time_out,
        const DG_S32 fn_start_time,
        const DG_U8 card_index
        )
{
    DG_U32 i;
    DG_U32 mod4;
    DG_U8 zero[4] = {0,0,0,0};
    DG_RESULT result;

    for (i = 1; i < send->nb_entry; i++) {
        if ((result = dg_hdio_transmit(dg_get_channel_for_protocol(protocol), send->nb_bytes[i], send->p_data[i], fn_time_out, fn_start_time, card_index)) != DG_OK) {
            hdc_debug_printf("[%d] hdio transmit error (%d) when sending payload entry %d\n", dg_get_channel_for_protocol(protocol), result, i);
            return result;
        }
        mod4 = send->nb_bytes[i] % 4;
        if (mod4 > 0) {
            if ((result = dg_hdio_transmit(dg_get_channel_for_protocol(protocol), (4 - mod4), zero, fn_time_out, fn_start_time, card_index))!= DG_OK) {
                hdc_debug_printf("[%d] hdio transmit error (%d) when sending payload entry %d alignment\n", dg_get_channel_for_protocol(protocol), result, i);
                return result;
            }
        }
    }
    return DG_OK;
}

RPC_DECL_LOCAL DG_RESULT dg_hdc_receive_payload (
        const DG_U8 protocol,
        const RPC_PROTOCOL_DATA* recv,
        const DG_U8 card_index
        )
{
    DG_RESULT result;
    DG_U32 i;
    DG_U32 mod4;
    DG_U8 zero[4] = {0,0,0,0};

    for (i = 0; i < recv->nb_entry ; i++) {
        if ((result = dg_hdio_receive(dg_get_channel_for_protocol(protocol), recv->nb_bytes[i], recv->p_data[i], card_index)) != DG_OK){
            hdc_debug_printf("[%d] hdio receive error (%d) on payload entry %d\n",dg_get_channel_for_protocol(protocol), result, i);
            return result;
        }
        mod4 = recv->nb_bytes[i] % 4;
        if (mod4>0) {
            if ((result = dg_hdio_receive(dg_get_channel_for_protocol(protocol), (4-mod4), zero, card_index))!= DG_OK)          {
                hdc_debug_printf("[%d] hdio receive error (%d) on payload entry %d alignment\n",dg_get_channel_for_protocol(protocol),result, i);
                return result;
            }
        }
    }
    /* check if there has been a reset during payload transfer */
    if((result = dg_hdio_sanity_check(dg_get_channel_for_protocol(protocol), card_index)) != DG_OK){
        hdc_debug_printf("[%d] hdio receive error (%d)\n",dg_get_channel_for_protocol(protocol), result);
    }
    return result;
}

RPC_DECL_LOCAL DG_RESULT dg_hdc_send(
        const DG_U8 protocol,
        const RTM_HANDLE_P handle,
        const RPC_PROTOCOL_DATA *send,
        const DG_S32 fn_time_out,
        const DG_S32 fn_start_time
        )
{
    DG_RESULT res;
    HEADER header;
    DG_U8 card_index=0;

    if (handle && handle->card_handle_defined) {
        card_index=handle->card_index;
    }
    if (handle && handle->rt_handle_defined && rtm_isWorking(handle)) { /* async polling call */
        header.command = rtm_get_handle(handle);
        header.payloadLen = 0;
        header.protocol = HDC_PROTOCOL_ASKWAIT;
        return dg_hdc_send_header(protocol, &header, fn_time_out, fn_start_time, card_index);
    }

    /* sync and first async call */
    header.payloadLen = dg_calculate_payload_length(1, send->nb_entry, send->nb_bytes);
    header.protocol = protocol;
    if (handle && handle->rt_handle_defined) {/* first async call */
        header.command = (DG_U8)(*(DG_U8*)send->p_data[0] | MASK_HANDLE_REQUEST);
    } else { /* sync call */
        header.command = *(DG_U8*)(send->p_data[0]);
    }
    if ((res = dg_hdc_send_header(protocol, &header, fn_time_out, fn_start_time, card_index))!=0)
        return res;

    return dg_hdc_send_payload(protocol, send, fn_time_out, fn_start_time, card_index);
}

RPC_DECL_LOCAL DG_RESULT dg_hdc_receive_header(const DG_U8 protocol, HEADER* header, DG_U8 card_index) {
    DG_U8 headerStream[4];
    DG_RESULT result;

    if ((result = dg_hdio_receive(dg_get_channel_for_protocol(protocol), 4, headerStream, card_index))!= DG_OK)    {
        hdc_debug_printf("[%d] hdio receive error (%d) on getting the header\n", dg_get_channel_for_protocol(protocol), result);
        return result;
    }

    header->protocol = headerStream[0];
    header->command = headerStream[1];
    header->payloadLen = ((DG_U16)headerStream[3] <<8) | headerStream[2];
    return DG_OK;
}

RPC_DECL_LOCAL
DG_RESULT
dg_hdc_receive
(
    DG_U8 protocol,
    RTM_HANDLE_P handle,
    const RPC_PROTOCOL_DATA *recv
)
{
    DG_RESULT res;
    HEADER header;
    DG_U16 len;
    DG_U8 card_index=0;

    if (handle && handle->card_handle_defined) {
        card_index=handle->card_index;
    }
    if ((res = dg_hdc_receive_header(protocol, &header, card_index))!=DG_OK)
        return res;

    if ((!handle || !handle->rt_handle_defined)&& protocol != header.protocol) {
        hdc_debug_printf("requested %d and received %d protocols do not match\n", protocol, header.protocol);
        return DG_ERROR_HDC_PROTOCOL_MISMATCH;
    }

    if (HDC_PROTOCOL_ERROR == header.protocol) {
        return DG_ERROR_HDC_PROTOCOL;
    }

    if (HDC_PROTOCOL_ASKWAIT == header.protocol) {
        if (!handle || !handle->rt_handle_defined) /* bad error: sync call initiated... async answer received*/
            return DG_ERROR_HDC_HANDLE;
        rtm_set_handle(handle, header.command);
        return DG_OK; /* waiting continues */
    }

    if (handle && handle->rt_handle_defined) /* async call finished */
        rtm_setFinished(handle);

    if (dg_card_config[card_index].hdc.recv_h_func!=NULL) {
        dg_card_config[card_index].hdc.recv_h_func(protocol, header.command, card_index);
    }
    len = dg_calculate_payload_length(0, recv->nb_entry, recv->nb_bytes);
    if (len != header.payloadLen) {
        hdc_debug_printf("%d %d payload len mismatch! received %d, expected: %d\n", protocol, header.command, header.payloadLen, len);
        if ( header.payloadLen == 0 )
        {
            hdc_debug_printf ("\nempty block received\n");
            return DG_ERROR_EMPTY_BLOCK;
        }
        else
        {
            return DG_ERROR_HDC_LENGTH_MISSMATCH;
        }
    }

    if ((res = dg_hdc_receive_payload(header.protocol, recv, card_index)) != DG_OK)
        return res;

    return DG_OK;
}

RPC_DECL_PUBLIC DG_RESULT
dg_hdc_private_request
(
    const DG_U8 channel,
    const DG_U8 protocol,
    const RTM_HANDLE_P handle,
    const RPC_PROTOCOL_DATA *send,
    const RPC_PROTOCOL_DATA *recv
)
{
    DG_RESULT res;
    OSL_MUTEX* mutex;
    DG_U8 i=0;
    DG_U8 command = *(DG_U8*)(send->p_data[0]);
    DG_S32 function_time_out;
    DG_S32 function_start_time = 0;
    DG_U8 card_index=0;

    if (handle && handle->card_handle_defined) {
        card_index=handle->card_index;
    }
    function_time_out = dg_card_config[card_index].dg_hdio_configs[channel].fn_timeout_default;
    mutex = dg_get_mutex_for_channel(card_index, channel);
    while ( time_outs_tab[i].protocol != LAST_ENTRY_MARKER )
    {
        if ( time_outs_tab[i].protocol == protocol )
        {
            if ( time_outs_tab[i].command == *(DG_U8*)(send->p_data[0]) )
            {
                function_time_out = time_outs_tab[i].time_out;
                break;
            }
        }
        i++;
    }
    hdc_debug_printf ("protocol %d Command %d time-out %0X \n", protocol, *(DG_U8*)(send->p_data[0]), function_time_out );
    if ((res = osl_lock_uart_mutex(mutex))!=DG_OK) {
        return res;
    }
    function_start_time = osl_get_us_time_stamp();
    if (dg_card_config[card_index].hdc.start_func!=NULL) {
        dg_card_config[card_index].hdc.start_func(protocol, command, card_index);
    }
    if ((res = dg_hdc_send(protocol, handle, send, function_time_out, function_start_time)) != DG_OK) {
        osl_unlock_uart_mutex(mutex);
        return res;
    }
    if (dg_card_config[card_index].hdc.send_func!=NULL) {
        dg_card_config[card_index].hdc.send_func(protocol, command, card_index);
    }
    if ((res = dg_hdc_receive(protocol, handle, recv)) !=DG_OK) {
        osl_unlock_uart_mutex(mutex);
        return res;
    }
    if (dg_card_config[card_index].hdc.recv_p_func!=NULL) {
        dg_card_config[card_index].hdc.recv_p_func(protocol, command, card_index);
    }
    if ((res = osl_unlock_uart_mutex(mutex))!=DG_OK) {
        return res;
    }
    hdc_debug_printf ( "Time to execute function was %d\n", osl_get_us_time_stamp() - function_start_time);
    return DG_OK;
}

RPC_DECL_PUBLIC
DG_RESULT dg_hdc_request
(
    DG_U8 protocol,
    RTM_HANDLE_P handle,
    RPC_PROTOCOL_DATA *send,
    RPC_PROTOCOL_DATA *recv
)
{
    RTM_HANDLE _myHandle;
    RTM_HANDLE_P myHandle = &_myHandle;
    DG_RESULT res = 0;
    DG_U8 channel = dg_get_channel_for_protocol(protocol);
    DG_U8 card_index=0;

    /* sanity check */
    if (channel > DG_NR_OF_CHANNELS - 1) {
        hdc_debug_printf ( "Error: protocol channel number (%d) greater than number of channels (DG_NR_OF_CHANNELS-1=%d)\n", channel, DG_NR_OF_CHANNELS-1);
        return DG_ERROR_COMM_FAIL;
    }
    if (handle && handle->card_handle_defined) {
        card_index=handle->card_index;
    }
    if(dg_card_config[card_index].channel_timeout[channel] == TRUE) {
        return DG_ERROR_CHANNEL_TIMED_OUT;
    }
    if ((!handle || !handle->rt_handle_defined) && dg_hdio_is_polling_channel(channel, card_index)){
        /* initially sync call on a polling channel */
        rtm_init_handle(myHandle);
        do {
            res = dg_hdc_private_request(channel, protocol, myHandle, send, recv);
        } while ((res==0) && rtm_isWorking(myHandle));
    } else {
        res = dg_hdc_private_request(channel, protocol, handle, send, recv);
    }
    if(res == DG_ERROR_FUNCTION_TIMED_OUT){
        dg_card_config[card_index].channel_timeout[channel] = TRUE;
    }
    return res;
}
