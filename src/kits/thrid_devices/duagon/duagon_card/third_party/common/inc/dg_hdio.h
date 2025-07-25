/* ==========================================================================
 *
 *  File      : DG_HDIO.H
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

#ifndef DG_HDIO_H
#define DG_HDIO_H

#include "tcn_am.h"
#include "dg_rtm.h"
#include "dg_rpc_g.h"
#include "tcn_def.h"
#include "os_def.h"
#include "dg_conf.h"
#include "dg_error.h"
#include "dg_ip.h"
#include "dg_perf.h"  /* PerfFunc */
#ifndef TT_FULL
#include "tcn_mcs_cgf.h"   /* MCS_CGF_GLOBALS needs this */
#include "tcn_mcs_def.h"  /* MCS_CLCH_GLOBALS */
#endif

#define HDIO_CONFIG_SIZE 32
/* design limits */
#define DG_MAX_CHAN_PER_CARD 4
#define DG_MAX_CARDS_PER_DRIVER 4
#define DG_NO_CARD 0xFF /* invalid card (number or index) */


#define DG_MDFULL_LP_PORT_NUMBER_MAX 4096
#ifdef MDFULL_LP_PORT_NUMBER_MAX
#if MDFULL_LP_PORT_NUMBER_MAX != DG_MDFULL_LP_PORT_NUMBER_MAX
#error /* inconsistent definitions */
#endif
#endif

#define DG_BLS_MAX_LINKS 16
#ifdef BLS_MAX_LINKS
#if BLS_MAX_LINKS != DG_BLS_MAX_LINKS
#error /* inconsistent definitions */
#endif
#endif



/* DG_IO_MODE setup */
#if (DG_IO_MODE==DG_HDCFG_PARALLEL)
#define DG_IO_MODE_ALL DG_HDCFG_PARALLEL
#endif
#if (DG_IO_MODE==DG_HDCFG_USB)
#define DG_IO_MODE_ALL DG_HDCFG_USB
#endif
#if (DG_IO_MODE==DG_HDCFG_SERIAL)
#define DG_IO_MODE_ALL DG_HDCFG_SERIAL
#endif

/* dg_io_mode macro */
#define DG_CARD_IO_MODE(card_index) dg_card_config[(card_index)].dg_io_mode

/* mvb_channel macro */
#define CARD_MVB_CHANNEL_NUM(card_index) dg_card_config[(card_index)].mvb_channel
#define CARD_MVB_SERVER_CHANNEL_NUM(card_index) dg_card_config[(card_index)].mvb_srv_channel
#define CARD_GET_INDEX(card_Number) card_defined[(card_Number)].index
#define CARD_IS_DEFINED(card_Number) card_defined[(card_Number)].defined
#define CARD_IS_VALID_INDEX(card_index) \
       ( (card_index) != DG_NO_CARD && ( \
       (card_defined[0].defined && card_defined[0].index==(card_index)) ||  \
       (card_defined[1].defined && card_defined[1].index==(card_index)) ||  \
       (card_defined[2].defined && card_defined[2].index==(card_index)) ||  \
       (card_defined[3].defined && card_defined[3].index==(card_index))   ) \
)? TRUE:FALSE


/* dg_hdio_common_config access macros */
#define LOCATE_CFG_FTO(c,i)     (&((GET_REG_CONFIG(c,i))->dg_hdio_common_config.function_timeout))
#define LOCATE_CFG_FST(c,i)     (&((GET_REG_CONFIG(c,i))->dg_hdio_common_config.function_start_time))

/* dg_card_config access macros */
#define DG_HOST_BASE_CHANNEL_0(card_index) dg_card_config[(card_index)].host_base_channel[0]
#if DG_NR_OF_CHANNELS < 2
#  define DG_HOST_BASE_CHANNEL_1(card_index) 0xD0E
#else
#  define DG_HOST_BASE_CHANNEL_1(card_index) dg_card_config[(card_index)].host_base_channel[1]
#endif
#if DG_NR_OF_CHANNELS < 3
#  define DG_HOST_BASE_CHANNEL_2(card_index) 0xD0E
#else
#  define DG_HOST_BASE_CHANNEL_2(card_index) dg_card_config[(card_index)].host_base_channel[2]
#endif
#if DG_NR_OF_CHANNELS < 4
#  define DG_HOST_BASE_CHANNEL_3(card_index) 0xD0E
#else
#  define DG_HOST_BASE_CHANNEL_3(card_index) dg_card_config[(card_index)].host_base_channel[3]
#endif



/* work out number of cards defined DG_NR_OF_CARDS_DEFINED */
#if CARD_3_TYPE==CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE
#   error no cards
#else
#   define DG_NR_OF_CARDS_DEFINED DG_MAX_CARDS_PER_DRIVER
#endif

/* three cards */
#if       CARD_0_TYPE==CARD_NONE \
       || CARD_1_TYPE==CARD_NONE \
       || CARD_2_TYPE==CARD_NONE \
       || CARD_3_TYPE==CARD_NONE
#   undef DG_NR_OF_CARDS_DEFINED
#   define DG_NR_OF_CARDS_DEFINED DG_MAX_CARDS_PER_DRIVER-1
#endif
/* two cards */
#if       CARD_1_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE \
       || CARD_2_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE \
       || CARD_3_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE \
       || CARD_2_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE \
       || CARD_3_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE \
       || CARD_3_TYPE==CARD_NONE && CARD_2_TYPE==CARD_NONE
#   undef DG_NR_OF_CARDS_DEFINED
#   define DG_NR_OF_CARDS_DEFINED DG_MAX_CARDS_PER_DRIVER-2
#endif
/* one cards */
#if       CARD_3_TYPE==CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE \
       || CARD_3_TYPE==CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE \
       || CARD_3_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE \
       || CARD_2_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE
#   undef DG_NR_OF_CARDS_DEFINED
#   define DG_NR_OF_CARDS_DEFINED DG_MAX_CARDS_PER_DRIVER-3
#endif


typedef enum {
    DG_HDIO_NOT_INITIALIZED,
    DG_HDIO_IS_INITIALIZED
} DG_HDIO_INITIALIZATION;

typedef struct {
    DG_U32 channel_type;
    DG_HDIO_INITIALIZATION is_initialized;
    DG_S32 read_timeout;
    DG_S32 write_timeout;
    DG_U32 blocking;
    void* handle;
    DG_S32 fn_timeout_default;
    DG_S32 function_timeout;
    DG_S32 function_start_time;
    DG_BOOL is_polling;
    DG_U8 config_data[HDIO_CONFIG_SIZE]; /* reserve empty space for access-specific configuration (PAR / SER / USB) */
} DG_HDIO_CONFIG;

typedef struct {
    DG_U32 channel_type;
    DG_HDIO_INITIALIZATION is_initialized;
    DG_S32 read_timeout;
    DG_S32 write_timeout;
    DG_U32 blocking;
    void* handle;
    DG_S32 fn_timeout_default;
    DG_S32 function_timeout;
    DG_S32 function_start_time;
    DG_BOOL is_polling;
} DG_HDIO_CONFIG_SHORT;

#define MAX_HOSTENT_ALIAS_SIZE 16
#define MAX_HOSTENT_ALIAS_BUFFER_SIZE 256
#define MAX_HOSTENT_ADDR_SIZE 8
#define MAX_HOSTENT_ADDR_BUFFER_SIZE (MAX_HOSTENT_ADDR_SIZE * 4)
#define MAX_HOSTENT_HOSTNAME_SIZE 64
#define SIZE_TABLE_SIZE 200 /* this limits the numbers of used comIds
                               set to PD MAX_SEND_PACKETS on device (DH)*/
#define ERROR_STRING_LEN 256

typedef struct {
    PerfFunc write_func;
    PerfFunc read_d_func;
    PerfFunc read_s_func;
    PerfFuncU8 is_polling_func;
} PERF_GLOBALS;



typedef struct {
    OSL_MUTEX ip_protocol_mutex; /* protocol mutex used by rpc.ch */
    DG_U8 ip_mutex_init_done;
    struct ip_hostent g_hostent;
    DG_CHAR8 g_hostent_in_addr[MAX_HOSTENT_ADDR_BUFFER_SIZE];
    DG_CHAR8 g_aliases[MAX_HOSTENT_ALIAS_BUFFER_SIZE];
    DG_CHAR8 g_hostname[MAX_HOSTENT_HOSTNAME_SIZE];
    DG_CHAR8 *g_aliasp[MAX_HOSTENT_ALIAS_SIZE];
    DG_CHAR8 *g_hostent_in_addrp[MAX_HOSTENT_ADDR_SIZE];
} IP_GLOBALS;

typedef struct {
    DG_U32 handle;
    DG_U32 size;
} PD_STRUCT_SIZETABLE;

typedef struct {
    OSL_MUTEX ipt_md_protocol_mutex; /* protocol mutex used by rpc.ch */
    DG_U8 ipt_md_mutex_init_done;
    DG_CHAR8 ipt_md_error_string[256];
    PD_STRUCT_SIZETABLE sizetable[SIZE_TABLE_SIZE];
    DG_CHAR8 ipt_pd_error_string[ERROR_STRING_LEN];
    OSL_MUTEX table_mutex;
} IPTCOM_GLOBALS;

typedef RPC_S32 (*DuagonSendFuncK) (RPC_U8, RTM_HANDLE_P handle, RPC_PROTOCOL_DATA *send, RPC_PROTOCOL_DATA *recv);
typedef void (*DuagonBoundaryFuncK)(RPC_U8 protocol, RPC_U8 command, DG_U8 card_index);

typedef struct {
    DuagonSendFuncK send_func;
    DuagonBoundaryFuncK in_func;
    DuagonBoundaryFuncK out_func;
} RPC_GLOBALS;

typedef struct {
    DuagonBoundaryFuncK start_func;
    DuagonBoundaryFuncK send_func;
    DuagonBoundaryFuncK recv_h_func;
    DuagonBoundaryFuncK recv_p_func;
} HDC_GLOBALS;

typedef struct {
    DuagonBoundaryFuncK in_func;
    DuagonBoundaryFuncK out_func;
} MVB_SERVER_GLOBALS;

typedef struct {
    DuagonBoundaryFuncK start_func;
    DuagonBoundaryFuncK send_func;
    DuagonBoundaryFuncK recv_h_func;
    DuagonBoundaryFuncK recv_p_func;
    UNSIGNED16 clp_supervision_interval;
    UNSIGNED16 clp_tick_counter_max;
    UNSIGNED8  clp_port_config_list_word_size[4096];
} MCS_CLP_GLOBALS;

typedef struct {
    DuagonBoundaryFuncK start_func;
    DuagonBoundaryFuncK send_func;
    DuagonBoundaryFuncK recv_h_func;
    DuagonBoundaryFuncK recv_p_func;
} MUE_PD_GLOBALS;



#if !defined TCN_AM_H && !defined MVB_MD_H
typedef UNSIGNED8 AM_RESULT;
typedef struct
{
   UNSIGNED8   sg_node;
   UNSIGNED8   func_or_stat;
   UNSIGNED8   next_station;
   UNSIGNED8   topo_counter;
}  AM_ADDRESS;

typedef void (*AM_RECEIVE_CONFIRM)
(
   UNSIGNED8            replier_function,
   const AM_ADDRESS *   caller,
   void *               in_msg_adr,
   UNSIGNED32           in_msg_size,
   void *               replier_ref
);

typedef void (*AM_CALL_CONFIRM)
(
   UNSIGNED8            caller_function,
   void *               am_caller_ref,
   const AM_ADDRESS *   replier,
   void *               in_msg_adr,
   UNSIGNED32           in_msg_size,
   AM_RESULT            status
);

typedef void (*AM_REPLY_CONFIRM)
(
   UNSIGNED8            replier_function,
   void *               replier_ref
);

#endif

typedef struct {
     DG_U32    ccb_package_counter;
     UNSIGNED8     ccb_return_number;
     BOOLEAN1      bCcbFirstPackage;
     BOOLEAN1      bCcbReceiveCrcError;
     UNSIGNED8 *   ccb_dynamic_data_base;
     UNSIGNED8 *   ccb_data_pointer;
     void *        ccb_in_msg_adr;
     UNSIGNED32    ccb_in_msg_size;
     UNSIGNED32    ccb_in_msg_size_counter;
     AM_RESULT     ccb_status;
    /******************************************************************************/
    /*   am_call_confirm                                                          */
    /******************************************************************************/
     AM_CALL_CONFIRM     ccb_call_confirm_function;
     UNSIGNED8           ccb_caller_function;
     void *              ccb_caller_ref;
     AM_ADDRESS          ccb_replier;
    /******************************************************************************/
    /*   am_receive_confirm                                                       */
    /******************************************************************************/
     AM_RECEIVE_CONFIRM  ccb_receive_confirm_function;
     UNSIGNED8           ccb_replier_function;
     AM_ADDRESS          ccb_caller;
     void *              ccb_replier_ref;
    /******************************************************************************/
    /*   am_reply_confirm                                                         */
    /******************************************************************************/
     AM_REPLY_CONFIRM    ccb_reply_confirm_function;
} MCS_CCB_GLOBALS;

#ifndef TT_FULL
typedef struct {
    UNSIGNED8 clch_return_number;

    /******************************************************************************/
    /*   Channel Package (send)                                                   */
    /*   - "channel_send_package" contains the package header and data            */
    /*   - the package header starts at index "MCS_CLIENT_CHANNEL_DUMMY_SIZE"     */
    /*     (NOTE: word alignment for package data)                                */
    /*   - the package data starts at index "CLCH_CHANNEL_SEND_HEADER_SIZE"       */
    /*   - "p_channel_send_package_data" points to the first value of             */
    /*     the package data                                                       */
    /*   - "p_channel_send_package_pointer" points to the current value of        */
    /*     the package data                                                       */
    /******************************************************************************/
    UNSIGNED16  channel_send_package[MCS_CLIENT_CHANNEL_NUMBER_MAX][MCS_CLIENT_CHANNEL_SEND_PACKAGE_SIZE/2];
    UNSIGNED8 * p_channel_send_package_pointer[MCS_CLIENT_CHANNEL_NUMBER_MAX];

    /******************************************************************************/
    /*   Channel Package (send)                                                   */
    /*   - "channel_send_package_data" contains the "send_package_header" and     */
    /*     the "send_package_data"; the "send_package_data" starts at             */
    /*     index "CLCH_CHANNEL_SEND_HEADER_SIZE"                                  */
    /*   - "channel_send_package_index" points to the "send_package_data" and has */
    /*     a range from 0 to (CLCH_CHANNEL_SEND_PACKAGE_SIZE - 1) (default = 0)   */
    /******************************************************************************/

    /******************************************************************************/
    /*   Channel Package (receive)                                                */
    /******************************************************************************/
#if defined (O_RECEIVE_CRC)
    UNSIGNED8 channel_receive_crc;
#endif
    BOOLEAN1 bChannelReceiveFirstPackage[MCS_CLIENT_CHANNEL_NUMBER_MAX];
    UNSIGNED8 channel_receive_return_number[MCS_CLIENT_CHANNEL_NUMBER_MAX];
} MCS_CLCH_GLOBALS;
#endif /*TT_FULL */

/******************************************************************************/
/*   Receive Channel                                                          */
/******************************************************************************/
#define CPIL_RECEICVE_BUFFER_SIZE       100


typedef struct {
#if !defined (O_OS_SINGLE)
    char cpil_dbg_str[80];
    /******************************************************************************/
    /*   Mutual Exclusion Semaphores                                              */
    /******************************************************************************/
    HANDLE   cpil_mutex[CPIL_MAX_MUTEX];

    /******************************************************************************/
    /*   Semaphores                                                               */
    /******************************************************************************/
    HANDLE   cpil_sem[CPIL_MAX_SEM];

    /******************************************************************************/
    /*   Threads                                                                  */
    /******************************************************************************/
    HANDLE   h_thread_supervisor;
    HANDLE   h_thread_callback;

#endif /* !O_OS_SINGLE */

    /******************************************************************************/
    /*   Receive Buffer                                                           */
    /******************************************************************************/
    UNSIGNED8    receive_buffer[CPIL_RECEICVE_BUFFER_SIZE + 1];
    UNSIGNED8    *p_receive_buffer_first;
    UNSIGNED8    *p_receive_buffer_last;
    UNSIGNED8    *p_receive_buffer_read;
} MCS_CPIL_GLOBALS;

#ifndef TT_FULL
typedef struct {
    DG_U16 cgf_device_address;
    UNSIGNED8 cgf_server_status;
    BOOLEAN1 bDone_gf_open_device;
    BOOLEAN1 bDone_gf_init_device;
    BOOLEAN1 bDone_gf_close_device;
    BOOLEAN1 bDone_gf_mvb_init;
    BOOLEAN1 bDone_gf_mvb_start;
    BOOLEAN1 bDone_lp_init;
    BOOLEAN1 bDone_am_init;
    BOOLEAN1 bDone_am_announce_device;
    BOOLEAN1 bDone_dsw_init;
    BOOLEAN1 bDone_ba_init;
    UNSIGNED16 cgf_device_status;
    UNSIGNED16 cgf_device_status_old;
    UNSIGNED16 cgf_device_status_supervisor;
#if defined (O_OS_SINGLE)
    BOOLEAN1 bCgfSupervisor;
#endif
    BOOLEAN1 bCgfCallbackBusy;
    BOOLEAN1 bStartUpServerDone;
    BOOLEAN1 bInitServerDone;
    BOOLEAN1 bInitPilDone;
    UNSIGNED32 cgf_client_life_sign_timeout;
    GF_DEVICE_STATUS_CALLBACK cgf_device_status_callback;
} MCS_CGF_GLOBALS;
#endif /*TT_FULL */

#define MCS_DIR_TABLE_SIZE    256

typedef struct {
    UNSIGNED8  mcs_dir_table[MCS_DIR_TABLE_SIZE];
} MCS_CMD_GLOBALS;

typedef struct {
    /* --------------------------------------------------------------------------
     *  Data buffer for communication channel
     * --------------------------------------------------------------------------
     */
    WORD8        cch_data_buf[35];
    UNSIGNED16   cch_data_cnt;
    WORD8       *cch_data_ptr;
} MUE_ACC_GLOBALS;

typedef struct {
   /* --------------------------------------------------------------------------
    *  Mutex for port access (read-modify-write)
    * --------------------------------------------------------------------------
    */
    BOOLEAN1 tcn_ap_mutex_status_port;
    OSL_MUTEX tcn_ap_mutex_object_port;

   /* --------------------------------------------------------------------------
    *  Test Process Variables
    * --------------------------------------------------------------------------
    */
#ifdef TCN_AP_TEST_VAR
    WORD8 tcn_ap_port_buffer[32];
#endif

   /* --------------------------------------------------------------------------
    *  Port Buffer
    * --------------------------------------------------------------------------
    */
#ifndef TCN_AP_TEST_VAR
    WORD8 tcn_ap_port_buffer[128];
#endif
    WORD8 *p_tcn_ap_port_buffer;
} TCN_AP_GLOBALS;

typedef struct {
    UNSIGNED16 cdsw_supervision_interval;
    UNSIGNED16 cdsw_tick_counter_max;
} MCS_CDSW_GLOBALS;

typedef struct {
    DG_PERF_MEASUREMENT_DEF dg_perf_measurements[DG_PERF_MAX_MEASUREMENTS];
    int dg_used_protocols[DG_PERF_MAX_MEASUREMENTS];
#ifdef MEASURE_DETAILED
    DG_U8 is_polling[DG_NR_OF_CHANNELS];
#endif
} PERF_COUNT_GLOBALS;

/* --------------------------------------------------------------------------
 *  DG place holder types for TCN_BLS_GLOBALS for more info see originals
 *  without DG_ prefix
 * --------------------------------------------------------------------------
 */


typedef enum { result } DG_FN_RESULT;

typedef struct
{
    WORD8       final[2];
    WORD8       origin[2];
} DG_MD_PACKET_NETWORK_HEADER;

typedef struct
{
    WORD8       mtc;
    WORD8       transport_data[1]; /* dynamic size */
} DG_MD_PACKET_NETWORK_DATA;

typedef struct
{
    DG_MD_PACKET_NETWORK_HEADER    network_header;
    DG_MD_PACKET_NETWORK_DATA      network_data;
} DG_MD_PACKET_LINK_DATA;

typedef struct
{
    WORD8               link_header[4];
    WORD8               link_header_sz;
    DG_MD_PACKET_LINK_DATA link_data;
} DG_MD_PACKET_FRAME_DATA;

typedef struct
{
    void                    *next;
    void                    *owner;
    WORD8                   control;
    ENUM8                   status;
    DG_MD_PACKET_FRAME_DATA frame_data;
} DG_MD_PACKET;

typedef struct
{
    UNSIGNED8   port_size;
    BITSET8     port_config;
    WORD16      reserved;
    void       *p_bus_specific;
} DG_PD_PRT_ATTR;


typedef void
(*DG_LM_STATUS_INDICATE)
(
    ENUM8         bus_id,
    DG_FN_RESULT  status
);

typedef void
(*DG_LM_SEND_CONFIRM)
(
    DG_MD_PACKET   *packet
);

typedef void
(*DG_LM_GET_PACK)
(
    void           **owner,
    DG_MD_PACKET   **packet
);

typedef void
(*DG_LM_RECEIVE_INDICATE)
(
    ENUM8       bus_id
);

typedef DG_FN_RESULT
(*DG_LM_RECEIVE_POLL)
(
    void          *p_bus_ctrl,
    UNSIGNED32    *source,
    UNSIGNED32    *destination,
    DG_MD_PACKET **packet,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LM_SEND_REQUEST)
(
    void         *p_bus_ctrl,
    UNSIGNED32    source,
    UNSIGNED32    destination,
    DG_MD_PACKET *packet,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LM_SEND_QUEUE_FLUSH)
(
    void        *p_bus_ctrl,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LM_GET_STATUS)
(
    void        *p_bus_ctrl,
    BITSET8      selector,
    BITSET8      reset,
    BITSET8     *status,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LM_GET_DEV_ADDRESS)
(
    void        *p_bus_ctrl,
    WORD32      *device_address,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LM_INIT)
(
    void                  *p_bus_ctrl,
    DG_LM_RECEIVE_INDICATE nm_receive_indicate,
    DG_LM_GET_PACK         nm_get_pack,
    void                 **owner,
    DG_LM_SEND_CONFIRM     nm_send_confirm,
    DG_LM_STATUS_INDICATE  nm_status_indicate,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LP_GET_DATASET)
(
    void        *p_bus_ctrl,
    WORD16       port_address,
    void        *p_value,
    void        *p_fresh,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LP_PUT_DATASET)
(
    void        *p_bus_ctrl,
    WORD16       port_address,
    void        *p_value,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LP_MANAGE)
(
    void           *p_bus_ctrl,
    WORD16          port_address,
    ENUM16          command,
    DG_PD_PRT_ATTR *prt_attr,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LP_INIT)
(
    void       *p_bus_ctrl,
    UNSIGNED16  fsi,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LS_INIT)
(
    void *p_bus_ctrl,
    DG_U8 card_index
);

typedef DG_FN_RESULT
(*DG_LS_SERVICE_HANDLER)
(
    void        *p_bus_ctrl,
    ENUM8        service,
    void        *p_parameter,
    DG_U8 card_index
);


typedef struct
{
    /* ----------------------------------------------------------------------
     *  LPBI procedures
     * ----------------------------------------------------------------------
     */
    DG_LP_INIT          lp_init;
    DG_LP_MANAGE        lp_manage;
    DG_LP_PUT_DATASET   lp_put_dataset;
    DG_LP_GET_DATASET   lp_get_dataset;
}   DG_BLS_DESC_PD;


typedef struct
{
    /* ----------------------------------------------------------------------
     *  LMBI procedures
     * ----------------------------------------------------------------------
     */
    DG_LM_INIT             lm_init;
    DG_LM_GET_DEV_ADDRESS  lm_get_dev_address;
    DG_LM_GET_STATUS       lm_get_status;
    DG_LM_SEND_QUEUE_FLUSH lm_send_queue_flush;
    DG_LM_SEND_REQUEST     lm_send_request;
    DG_LM_RECEIVE_POLL     lm_receive_poll;

    /* ----------------------------------------------------------------------
     *  subscribed procedures to the LMBI
     * ----------------------------------------------------------------------
     */
    DG_LM_RECEIVE_INDICATE lm_receive_indicate;
    DG_LM_GET_PACK         lm_get_pack;
    void                 **lm_packet_pool;
    DG_LM_SEND_CONFIRM     lm_send_confirm;
    DG_LM_STATUS_INDICATE  lm_status_indicate;
}   DG_BLS_DESC_MD;



typedef struct
{
    /* ----------------------------------------------------------------------
     *  LSBI procedures
     * ----------------------------------------------------------------------
     */
    DG_LS_INIT             ls_init;
    DG_LS_SERVICE_HANDLER  ls_service_handler;
} DG_BLS_DESC_SV;



typedef struct
{
    /* ----------------------------------------------------------------------
     *  bus controller description
     * ----------------------------------------------------------------------
     */
    void        *p_bus_ctrl;

    /* ----------------------------------------------------------------------
     *  link description
     * ----------------------------------------------------------------------
     */
    STRING32    link_name;
    ENUM8       link_type;

    /* ----------------------------------------------------------------------
     *  service descriptors
     * ----------------------------------------------------------------------
     */
    DG_BLS_DESC_SV desc_sv;
    DG_BLS_DESC_PD desc_pd;
    DG_BLS_DESC_MD desc_md;

} DG_BLS_DESC;

/*---------------------------------------------------------------------------*/


/* --------------------------------------------------------------------------
 *  Public Variable   : bls_descr_table
 *
 *  Purpose           : Points to the connected bus controller link layer
 *                      descriptors.
 *
 *  Remarks           : - The link identifier (e.g. link_id) is the index to
 *                        the table of bus controller link layer descriptor
 *                        pointers. If a pointer is NULL, then there is no
 *                        connected bus controller link layer.
 * --------------------------------------------------------------------------
 */


typedef struct
{
    /* ----------------------------------------------------------------------
     *  UART (static values)
     * ----------------------------------------------------------------------
     */
    WORD32      acc_reg_rbr_thr;
    WORD32      acc_reg_lsr;
    UNSIGNED32  acc_reg_size;
    UNSIGNED32  acc_reg_timeout;

    /* ----------------------------------------------------------------------
     *  LS (static values)
     * ----------------------------------------------------------------------
     */
    BOOLEAN1    ls_ds_capability;
    BOOLEAN1    ls_ba_capability;
    UNSIGNED16  ls_ba_list_entries;

    /* ----------------------------------------------------------------------
     *  MUE (dynamic values)
     * ----------------------------------------------------------------------
     */
    DG_FN_RESULT  mue_result;
    BITSET16      mue_sv_device_config;

    /* ----------------------------------------------------------------------
     *  Error Counters (dynamic values)
     * ----------------------------------------------------------------------
     */
    /* error counters - global                                             */
    UNSIGNED32  errcnt_global_lineA;
    UNSIGNED32  errcnt_global_lineB;

#ifdef MUELL_BC_ERRCNT_ALL_DS
    /* error counters - all device status ports                            */
    UNSIGNED32  errcnt_ds_port_lineA[MUELL_LS_DEVICE_NUMBER_MAX];
    UNSIGNED32  errcnt_ds_port_lineB[MUELL_LS_DEVICE_NUMBER_MAX];
#endif /* #ifdef MUELL_BC_ERRCNT_ALL_DS */

#ifdef MDFULL_BC_ERRCNT_ALL_PD
    /* error counters - all process data ports                             */
    UNSIGNED32  errcnt_pd_port_lineA[DG_MDFULL_LP_PORT_NUMBER_MAX];
    UNSIGNED32  errcnt_pd_port_lineB[DG_MDFULL_LP_PORT_NUMBER_MAX];
#endif /* #ifdef MDFULL_BC_ERRCNT_ALL_PD */

    /* ----------------------------------------------------------------------
     *  OSL (dynamic values)
     * ----------------------------------------------------------------------
     */
    BOOLEAN1        osl_mutex_status_uart;
    OSL_MUTEX      osl_mutex_object_uart;

    /* ----------------------------------------------------------------------
     *  LP (dynamic values)
     * ----------------------------------------------------------------------
     */
    UNSIGNED16  lp_fsi;
    UNSIGNED8   lp_size[DG_MDFULL_LP_PORT_NUMBER_MAX];
 
}   DG_MDFULL_BUS_CTRL;

typedef struct {
    DG_MDFULL_BUS_CTRL bc_mdfl;
    DG_BLS_DESC bls_desc_0;
    DG_BLS_DESC *bls_descr_table[DG_BLS_MAX_LINKS];
} TCN_BLS_GLOBALS;

#ifndef TT_FULL
/*
 * DG_CARD_CONFIG configuration and global info. for each card
 */
typedef struct {
    DG_U8 card_number;
    DG_HDIO_CONFIG dg_hdio_configs[DG_NR_OF_CHANNELS];
    DG_U32 host_base_channel[DG_NR_OF_CHANNELS];
    DG_U8 dg_io_mode;
    DG_HDIO_INITIALIZATION is_initialized;
    DG_U8 mvb_channel;
    DG_U8 mvb_srv_channel;
    DG_BOOL channel_timeout[DG_NR_OF_CHANNELS];
    OSL_MUTEX channel_mutex[DG_NR_OF_CHANNELS];
    DG_BOOL hdc_init_done;
    DG_BOOL chan_is_initialized;
    OSL_MUTEX hdio_init_mutex;
    IP_GLOBALS ip;
    IPTCOM_GLOBALS ic;
    PERF_GLOBALS p1g;
    PERF_GLOBALS p2g;
    PERF_GLOBALS p3g;
    RPC_GLOBALS rpc;
    HDC_GLOBALS hdc;
    PERF_COUNT_GLOBALS prf;
    MVB_SERVER_GLOBALS msv;
    MCS_CLP_GLOBALS mscp;
    MCS_CCB_GLOBALS mscb;
    MCS_CGF_GLOBALS mscf;
    MCS_CLCH_GLOBALS msch;
    MCS_CPIL_GLOBALS mscl;
    MCS_CMD_GLOBALS mscd;
    MCS_CDSW_GLOBALS mscw;
    MUE_PD_GLOBALS mupd;
    MUE_ACC_GLOBALS muac;
    TCN_AP_GLOBALS tcap;
    TCN_BLS_GLOBALS tcbs;
#ifdef MEASURE_BYTE_ACCESS
    BYTE_ACCESS_MEASUREMENTS bam;
#endif
} DG_CARD_CONFIG;

#endif /*TT_FULL */

#ifndef DG_MULTI_CARD
/* for backwards compatibility */
#define bls_desc_table (dg_card_config[0].tcbs.bls_descr_table)
#endif


/******************************************************************************************************
 *
 *  the array card_defined is set-up at compile time in accordance with definitions made in dg_conf.h
 *  it is declared with initializer INIT_CARD_DEF in dg_hdio.c
 *  INIT_CARD_DEF is defined below
 *
 *                      BOOL defined   U8 index
 *   card_defined[0]   |   true      |     0      |
 *   card_defined[1]   |   false     |     x      |
 *   card_defined[2]   |   false     |     x      |
 *   card_defined[3]   |   true      |     1      |
 *
 *  the size of the array dg_card_config is determined at compile time. The memory requirements
 *  per card are considerable so it is limited to the number of cards actually used.
 *
 *                      U8 card_number (see DG_CARD_CONFIG for further elements)
 *   dg_card_config[0] |      0       |             |...
 *   dg_card_config[1] |      3       |             |...
 *
 *   In the example the cards 0 and 3 have been defined
 *
 ******************************************************************************************************/

#ifndef TT_FULL

DG_DECL_PUBLIC DG_CARD_CONFIG dg_card_config[DG_NR_OF_CARDS_DEFINED]; /* one for each card used */
#endif /*TT_FULL */

typedef struct {
    DG_BOOL defined;
    DG_U8 index;
    DG_BOOL card_init_done;
} DG_CARD_DEFINED;

DG_DECL_PUBLIC DG_CARD_DEFINED card_defined[DG_MAX_CARDS_PER_DRIVER]; /* one for each possible cards */



#if       CARD_3_TYPE==CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE!=CARD_NONE
#define INIT_CARD_DEF {{1,0,0},{0,0,0},{0,0,0},{0,0,0}}
#endif
#if       CARD_3_TYPE==CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE!=CARD_NONE && CARD_0_TYPE==CARD_NONE
#define INIT_CARD_DEF {{0,0,0},{1,0,0},{0,0,0},{0,0,0}}
#endif
#if       CARD_3_TYPE==CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE!=CARD_NONE && CARD_0_TYPE!=CARD_NONE
#define INIT_CARD_DEF {{1,0,0},{1,1,0},{0,0,0},{0,0,0}}
#endif
#if       CARD_3_TYPE==CARD_NONE && CARD_2_TYPE!=CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE
#define INIT_CARD_DEF {{0,0,0},{0,0,0},{1,0,0},{0,0,0}}
#endif
#if       CARD_3_TYPE==CARD_NONE && CARD_2_TYPE!=CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE!=CARD_NONE
#define INIT_CARD_DEF {{1,0,0},{0,0,0},{1,1,0},{0,0,0}}
#endif
#if       CARD_3_TYPE==CARD_NONE && CARD_2_TYPE!=CARD_NONE && CARD_1_TYPE!=CARD_NONE && CARD_0_TYPE==CARD_NONE
#define INIT_CARD_DEF {{0,0,0},{1,0,0},{1,1,0},{0,0,0}}
#endif
#if       CARD_3_TYPE==CARD_NONE && CARD_2_TYPE!=CARD_NONE && CARD_1_TYPE!=CARD_NONE && CARD_0_TYPE!=CARD_NONE
#define INIT_CARD_DEF {{1,0,0},{1,1,0},{1,2,0},{0,0,0}}
#endif
#if       CARD_3_TYPE!=CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE
#define INIT_CARD_DEF {{0,0,0},{0,0,0},{0,0,0},{1,0,0}}
#endif
#if       CARD_3_TYPE!=CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE!=CARD_NONE
#define INIT_CARD_DEF {{1,0,0},{0,0,0},{0,0,0},{1,1,0}}
#endif
#if       CARD_3_TYPE!=CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE!=CARD_NONE && CARD_0_TYPE==CARD_NONE
#define INIT_CARD_DEF {{0,0,0},{1,0,0},{0,0,0},{1,1,0}}
#endif
#if       CARD_3_TYPE!=CARD_NONE && CARD_2_TYPE==CARD_NONE && CARD_1_TYPE!=CARD_NONE && CARD_0_TYPE!=CARD_NONE
#define INIT_CARD_DEF {{1,0,0},{1,1,0},{0,0,0},{1,2,0}}
#endif
#if       CARD_3_TYPE!=CARD_NONE && CARD_2_TYPE!=CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE==CARD_NONE
#define INIT_CARD_DEF {{0,0,0},{0,0,0},{1,0,0},{1,1,0}}
#endif
#if       CARD_3_TYPE!=CARD_NONE && CARD_2_TYPE!=CARD_NONE && CARD_1_TYPE==CARD_NONE && CARD_0_TYPE!=CARD_NONE
#define INIT_CARD_DEF {{1,0,0},{0,0,0},{1,1,0},{1,2,0}}
#endif
#if       CARD_3_TYPE!=CARD_NONE && CARD_2_TYPE!=CARD_NONE && CARD_1_TYPE!=CARD_NONE && CARD_0_TYPE==CARD_NONE
#define INIT_CARD_DEF {{0,0,0},{1,0,0},{1,1,0},{1,2,0}}
#endif
#if       CARD_3_TYPE!=CARD_NONE && CARD_2_TYPE!=CARD_NONE && CARD_1_TYPE!=CARD_NONE && CARD_0_TYPE!=CARD_NONE
#define INIT_CARD_DEF {{1,0,0},{1,1,0},{1,2,0},{1,3,0}}
#endif





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


/*
 * function prototypes
 */




DG_DECL_PUBLIC
DG_RESULT
dg_hdio_mvb_cards_init(void);

DG_DECL_PUBLIC
DG_RESULT
dg_hdio_init(
    DG_U8 channel_num,
    DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdio_transmit(
    DG_U8 channel_num,
    DG_U32 size,
    DG_U8* txBuff,
    const DG_S32 fn_time_out,
    const DG_S32 fn_start_time,
    DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdio_receive(
    DG_U8 channel_num,
    DG_U32 size,
    DG_U8* rxBuff,
    DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdio_wait_to_send(
   DG_U8 channel_num
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdio_set_loopback(
   DG_U8 channel_num,
   DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdio_release_loopback(
    DG_U8 channel_num,
    DG_U8 card_index
);

DG_DECL_PUBLIC
DG_BOOL
dg_hdio_is_polling_channel(
    DG_U8 channel_num,
    DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
dg_hdio_sanity_check(
    DG_U8 channel_num,
    DG_U8 card_index
);

/* macros to load parameters */


#define DG_CARD_Y_CHANNEL_X_MODE(INDEX,CARD)  DG_CARD_##CARD##_CHANNEL_##INDEX##_MODE
#define DG_CARD_Y_CHANNEL_X_HOST_BASE(INDEX,CARD)  DG_CARD_##CARD##_CHANNEL_##INDEX##_HOST_BASE
#define DG_CARD_Y_CHANNEL_X_READ_TIMEOUT(INDEX,CARD) DG_CARD_##CARD##_CHANNEL_##INDEX##_READ_TIMEOUT
#define DG_CARD_Y_CHANNEL_X_WRITE_TIMEOUT(INDEX,CARD) DG_CARD_##CARD##_CHANNEL_##INDEX##_WRITE_TIMEOUT
#define DG_CARD_Y_CHANNEL_X_BLOCKING(INDEX,CARD) DG_CARD_##CARD##_CHANNEL_##INDEX##_BLOCKING
#define DG_CARD_Y_CHANNEL_X_POLLING(INDEX,CARD) DG_CARD_##CARD##_CHANNEL_##INDEX##_POLLING
#define DG_CARD_Y_CHANNEL_X_FN_TO_DEFAULT(INDEX,CARD) DG_CARD_##CARD##_CHANNEL_##INDEX##_FN_TO_DEFAULT

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
#   if defined DG_CARD_0_MVB_CHANNEL
#       if DG_CARD_0_MVB_CHANNEL==0
#           undef DG_CARD_0_CHANNEL_0_MODE
#           define DG_CARD_0_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_0_MVB_CHANNEL==1
#           undef DG_CARD_0_CHANNEL_1_MODE
#           define DG_CARD_0_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_0_MVB_CHANNEL==2
#           undef DG_CARD_0_CHANNEL_2_MODE
#           define DG_CARD_0_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_0_MVB_CHANNEL==3
#           undef DG_CARD_0_CHANNEL_3_MODE
#           define DG_CARD_0_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   endif
#endif
#if CARD_1_TYPE==CARD_MVB
#   if defined DG_CARD_1_MVB_CHANNEL
#       if DG_CARD_1_MVB_CHANNEL==0
#           undef DG_CARD_1_CHANNEL_0_MODE
#           define DG_CARD_1_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_1_MVB_CHANNEL==1
#           undef DG_CARD_1_CHANNEL_1_MODE
#           define DG_CARD_1_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_1_MVB_CHANNEL==2
#           undef DG_CARD_1_CHANNEL_2_MODE
#           define DG_CARD_1_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_1_MVB_CHANNEL==3
#           undef DG_CARD_1_CHANNEL_3_MODE
#           define DG_CARD_1_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   endif
#endif
#if CARD_2_TYPE==CARD_MVB
#   if defined DG_CARD_2_MVB_CHANNEL
#       if DG_CARD_2_MVB_CHANNEL==0
#           undef DG_CARD_2_CHANNEL_0_MODE
#           define DG_CARD_2_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_2_MVB_CHANNEL==1
#           undef DG_CARD_2_CHANNEL_1_MODE
#           define DG_CARD_2_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_2_MVB_CHANNEL==2
#           undef DG_CARD_2_CHANNEL_2_MODE
#           define DG_CARD_2_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_2_MVB_CHANNEL==3
#           undef DG_CARD_2_CHANNEL_3_MODE
#           define DG_CARD_2_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   endif
#endif
#if CARD_3_TYPE==CARD_MVB
#   if defined DG_CARD_3_MVB_CHANNEL
#       if DG_CARD_3_MVB_CHANNEL==0
#           undef DG_CARD_3_CHANNEL_0_MODE
#           define DG_CARD_3_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_3_MVB_CHANNEL==1
#           undef DG_CARD_3_CHANNEL_1_MODE
#           define DG_CARD_3_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_3_MVB_CHANNEL==2
#           undef DG_CARD_3_CHANNEL_2_MODE
#           define DG_CARD_3_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_3_MVB_CHANNEL==3
#           undef DG_CARD_3_CHANNEL_3_MODE
#           define DG_CARD_3_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   endif
#endif

#if CARD_0_TYPE==SERVER2
#   if defined DG_CARD_0_MVB_CHANNEL
#       if DG_CARD_0_MVB_CHANNEL==0
#           undef DG_CARD_0_CHANNEL_0_MODE
#           define DG_CARD_0_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_0_MVB_CHANNEL==1
#           undef DG_CARD_0_CHANNEL_1_MODE
#           define DG_CARD_0_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_0_MVB_CHANNEL==2
#           undef DG_CARD_0_CHANNEL_2_MODE
#           define DG_CARD_0_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_0_MVB_CHANNEL==3
#           undef DG_CARD_0_CHANNEL_3_MODE
#           define DG_CARD_0_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   else
#        error "SERVER2 requires DG_CARD_0_MVB_CHANNEL and DG_CARD_0_MVB_SERVER_CHANNEL"
#   endif
#   if defined DG_CARD_0_MVB_SERVER_CHANNEL
#       if DG_CARD_0_MVB_SERVER_CHANNEL==0
#           undef DG_CARD_0_CHANNEL_0_MODE
#           ifdef D017TYPE
#               define DG_CARD_0_CHANNEL_0_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_0_CHANNEL_0_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_0_MVB_SERVER_CHANNEL==1
#           undef DG_CARD_0_CHANNEL_1_MODE
#           ifdef D017TYPE
#               define DG_CARD_0_CHANNEL_1_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_0_CHANNEL_1_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_0_MVB_SERVER_CHANNEL==2
#           undef DG_CARD_0_CHANNEL_2_MODE
#           ifdef D017TYPE
#               define DG_CARD_0_CHANNEL_2_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_0_CHANNEL_2_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_0_MVB_SERVER_CHANNEL==3
#           undef DG_CARD_0_CHANNEL_3_MODE
#           ifdef D017TYPE
#               define DG_CARD_0_CHANNEL_3_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_0_CHANNEL_3_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#   else
#        error "SERVER2 requires DG_CARD_0_MVB_CHANNEL and DG_CARD_0_MVB_SERVER_CHANNEL"
#   endif
#   if (DG_CARD_0_MVB_SERVER_CHANNEL == DG_CARD_0_MVB_CHANNEL )
#        error "DG_CARD_0_MVB_CHANNEL and DG_CARD_0_MVB_SERVER_CHANNEL must be different"
#   endif
#endif

#if CARD_1_TYPE==SERVER2
#   if defined DG_CARD_1_MVB_CHANNEL
#       if DG_CARD_1_MVB_CHANNEL==0
#           undef DG_CARD_1_CHANNEL_0_MODE
#           define DG_CARD_1_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_1_MVB_CHANNEL==1
#           undef DG_CARD_1_CHANNEL_1_MODE
#           define DG_CARD_1_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_1_MVB_CHANNEL==2
#           undef DG_CARD_1_CHANNEL_2_MODE
#           define DG_CARD_1_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_1_MVB_CHANNEL==3
#           undef DG_CARD_1_CHANNEL_3_MODE
#           define DG_CARD_1_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   else
#        error "SERVER2 requires DG_CARD_1_MVB_CHANNEL and DG_CARD_1_MVB_SERVER_CHANNEL"
#   endif
#   if defined DG_CARD_1_MVB_SERVER_CHANNEL
#       if DG_CARD_1_MVB_SERVER_CHANNEL==0
#           undef DG_CARD_1_CHANNEL_0_MODE
#           ifdef D017TYPE
#               define DG_CARD_1_CHANNEL_0_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_1_CHANNEL_0_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_1_MVB_SERVER_CHANNEL==1
#           undef DG_CARD_1_CHANNEL_1_MODE
#           ifdef D017TYPE
#               define DG_CARD_1_CHANNEL_1_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_1_CHANNEL_1_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_1_MVB_SERVER_CHANNEL==2
#           undef DG_CARD_1_CHANNEL_2_MODE
#           ifdef D017TYPE
#               define DG_CARD_1_CHANNEL_2_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_1_CHANNEL_2_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_1_MVB_SERVER_CHANNEL==3
#           undef DG_CARD_1_CHANNEL_3_MODE
#           ifdef D017TYPE
#               define DG_CARD_1_CHANNEL_3_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_1_CHANNEL_3_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#   else
#        error "SERVER2 requires DG_CARD_1_MVB_CHANNEL and DG_CARD_1_MVB_SERVER_CHANNEL"
#   endif
#   if (DG_CARD_1_MVB_SERVER_CHANNEL == DG_CARD_1_MVB_CHANNEL )
#        error "DG_CARD_1_MVB_CHANNEL and DG_CARD_1_MVB_SERVER_CHANNEL must be different"
#   endif
#endif

#if CARD_2_TYPE==SERVER2
#   if defined DG_CARD_2_MVB_CHANNEL
#       if DG_CARD_2_MVB_CHANNEL==0
#           undef DG_CARD_2_CHANNEL_0_MODE
#           define DG_CARD_2_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_2_MVB_CHANNEL==1
#           undef DG_CARD_2_CHANNEL_1_MODE
#           define DG_CARD_2_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_2_MVB_CHANNEL==2
#           undef DG_CARD_2_CHANNEL_2_MODE
#           define DG_CARD_2_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_2_MVB_CHANNEL==3
#           undef DG_CARD_2_CHANNEL_3_MODE
#           define DG_CARD_2_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   else
#        error "SERVER2 requires DG_CARD_2_MVB_CHANNEL and DG_CARD_2_MVB_SERVER_CHANNEL"
#   endif
#   if defined DG_CARD_2_MVB_SERVER_CHANNEL
#       if DG_CARD_2_MVB_SERVER_CHANNEL==0
#           undef DG_CARD_2_CHANNEL_0_MODE
#           ifdef D017TYPE
#               define DG_CARD_2_CHANNEL_0_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_2_CHANNEL_0_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_2_MVB_SERVER_CHANNEL==1
#           undef DG_CARD_2_CHANNEL_1_MODE
#           ifdef D017TYPE
#               define DG_CARD_2_CHANNEL_1_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_2_CHANNEL_1_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_2_MVB_SERVER_CHANNEL==2
#           undef DG_CARD_2_CHANNEL_2_MODE
#           ifdef D017TYPE
#               define DG_CARD_2_CHANNEL_2_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_2_CHANNEL_2_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_2_MVB_SERVER_CHANNEL==3
#           undef DG_CARD_2_CHANNEL_3_MODE
#           ifdef D017TYPE
#               define DG_CARD_2_CHANNEL_3_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_2_CHANNEL_3_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#   else
#        error "SERVER2 requires DG_CARD_2_MVB_CHANNEL and DG_CARD_2_MVB_SERVER_CHANNEL"
#   endif
#   if (DG_CARD_2_MVB_SERVER_CHANNEL == DG_CARD_2_MVB_CHANNEL )
#        error "DG_CARD_2_MVB_CHANNEL and DG_CARD_2_MVB_SERVER_CHANNEL must be different"
#   endif
#endif

#if CARD_3_TYPE==SERVER2
#   if defined DG_CARD_3_MVB_CHANNEL
#       if DG_CARD_3_MVB_CHANNEL==0
#           undef DG_CARD_3_CHANNEL_0_MODE
#           define DG_CARD_3_CHANNEL_0_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_3_MVB_CHANNEL==1
#           undef DG_CARD_3_CHANNEL_1_MODE
#           define DG_CARD_3_CHANNEL_1_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_3_MVB_CHANNEL==2
#           undef DG_CARD_3_CHANNEL_2_MODE
#           define DG_CARD_3_CHANNEL_2_MODE DG_HDCFG_CHANNEL_1G
#       endif
#       if DG_CARD_3_MVB_CHANNEL==3
#           undef DG_CARD_3_CHANNEL_3_MODE
#           define DG_CARD_3_CHANNEL_3_MODE DG_HDCFG_CHANNEL_1G
#       endif
#   else
#        error "SERVER2 requires DG_CARD_3_MVB_CHANNEL and DG_CARD_3_MVB_SERVER_CHANNEL"
#   endif
#   if defined DG_CARD_3_MVB_SERVER_CHANNEL
#       if DG_CARD_3_MVB_SERVER_CHANNEL==0
#           undef DG_CARD_3_CHANNEL_0_MODE
#           ifdef D017TYPE
#               define DG_CARD_3_CHANNEL_0_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_3_CHANNEL_0_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_3_MVB_SERVER_CHANNEL==1
#           undef DG_CARD_3_CHANNEL_1_MODE
#           ifdef D017TYPE
#               define DG_CARD_3_CHANNEL_1_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_3_CHANNEL_1_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_3_MVB_SERVER_CHANNEL==2
#           undef DG_CARD_3_CHANNEL_2_MODE
#           ifdef D017TYPE
#               define DG_CARD_3_CHANNEL_2_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_3_CHANNEL_2_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#       if DG_CARD_3_MVB_SERVER_CHANNEL==3
#           undef DG_CARD_3_CHANNEL_3_MODE
#           ifdef D017TYPE
#               define DG_CARD_3_CHANNEL_3_MODE DG_HDCFG_CHANNEL_2G
#           else
#               define DG_CARD_3_CHANNEL_3_MODE DG_HDCFG_CHANNEL_3G
#           endif
#       endif
#   else
#        error "SERVER2 requires DG_CARD_3_MVB_CHANNEL and DG_CARD_3_MVB_SERVER_CHANNEL"
#   endif
#   if (DG_CARD_3_MVB_SERVER_CHANNEL == DG_CARD_3MVB_CHANNEL )
#        error "DG_CARD_3_MVB_CHANNEL and DG_CARD_3_MVB_SERVER_CHANNEL must be different"
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
#ifndef DG_CARD_0_CHANNEL_2_READ_TIMEOUT
#define DG_CARD_0_CHANNEL_2_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_1_CHANNEL_2_READ_TIMEOUT
#define DG_CARD_1_CHANNEL_2_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_2_CHANNEL_2_READ_TIMEOUT
#define DG_CARD_2_CHANNEL_2_READ_TIMEOUT 3000
#endif
#ifndef DG_CARD_3_CHANNEL_2_READ_TIMEOUT
#define DG_CARD_3_CHANNEL_2_READ_TIMEOUT 3000
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
#ifndef DG_CARD_0_CHANNEL_2_WRITE_TIMEOUT
#define DG_CARD_0_CHANNEL_2_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_1_CHANNEL_2_WRITE_TIMEOUT
#define DG_CARD_1_CHANNEL_2_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_2_CHANNEL_2_WRITE_TIMEOUT
#define DG_CARD_2_CHANNEL_2_WRITE_TIMEOUT 1000
#endif
#ifndef DG_CARD_3_CHANNEL_2_WRITE_TIMEOUT
#define DG_CARD_3_CHANNEL_2_WRITE_TIMEOUT 1000
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
#ifndef DG_CARD_0_CHANNEL_2_FN_TO_DEFAULT
#define DG_CARD_0_CHANNEL_2_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_1_CHANNEL_2_FN_TO_DEFAULT
#define DG_CARD_1_CHANNEL_2_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_2_CHANNEL_2_FN_TO_DEFAULT
#define DG_CARD_2_CHANNEL_2_FN_TO_DEFAULT 30000000
#endif
#ifndef DG_CARD_3_CHANNEL_2_FN_TO_DEFAULT
#define DG_CARD_3_CHANNEL_2_FN_TO_DEFAULT 30000000
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
#ifndef DG_CARD_0_SERIAL_CHANNEL_2_COM_NR
#define DG_CARD_0_SERIAL_CHANNEL_2_COM_NR 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_2_COM_NR
#define DG_CARD_1_SERIAL_CHANNEL_2_COM_NR 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_2_COM_NR
#define DG_CARD_2_SERIAL_CHANNEL_2_COM_NR 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_2_COM_NR
#define DG_CARD_3_SERIAL_CHANNEL_2_COM_NR 1
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
#ifndef DG_CARD_0_SERIAL_CHANNEL_2_BAUD
#define DG_CARD_0_SERIAL_CHANNEL_2_BAUD 115200
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_2_BAUD
#define DG_CARD_1_SERIAL_CHANNEL_2_BAUD 115200
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_2_BAUD
#define DG_CARD_2_SERIAL_CHANNEL_2_BAUD 115200
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_2_BAUD
#define DG_CARD_3_SERIAL_CHANNEL_2_BAUD 115200
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
#ifndef DG_CARD_0_SERIAL_CHANNEL_2_DATABITS
#define DG_CARD_0_SERIAL_CHANNEL_2_DATABITS 8
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_2_DATABITS
#define DG_CARD_1_SERIAL_CHANNEL_2_DATABITS 8
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_2_DATABITS
#define DG_CARD_2_SERIAL_CHANNEL_2_DATABITS 8
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_2_DATABITS
#define DG_CARD_3_SERIAL_CHANNEL_2_DATABITS 8
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
#ifndef DG_CARD_0_SERIAL_CHANNEL_2_PARITY
#define DG_CARD_0_SERIAL_CHANNEL_2_PARITY 0
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_2_PARITY
#define DG_CARD_1_SERIAL_CHANNEL_2_PARITY 0
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_2_PARITY
#define DG_CARD_2_SERIAL_CHANNEL_2_PARITY 0
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_2_PARITY
#define DG_CARD_3_SERIAL_CHANNEL_2_PARITY 0
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
#ifndef DG_CARD_0_SERIAL_CHANNEL_2_STOPBITS
#define DG_CARD_0_SERIAL_CHANNEL_2_STOPBITS 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_2_STOPBITS
#define DG_CARD_1_SERIAL_CHANNEL_2_STOPBITS 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_2_STOPBITS
#define DG_CARD_2_SERIAL_CHANNEL_2_STOPBITS 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_2_STOPBITS
#define DG_CARD_3_SERIAL_CHANNEL_2_STOPBITS 1
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
#ifndef DG_CARD_0_SERIAL_CHANNEL_2_HANDSHAKE
#define DG_CARD_0_SERIAL_CHANNEL_2_HANDSHAKE 1
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_2_HANDSHAKE
#define DG_CARD_1_SERIAL_CHANNEL_2_HANDSHAKE 1
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_2_HANDSHAKE
#define DG_CARD_2_SERIAL_CHANNEL_2_HANDSHAKE 1
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_2_HANDSHAKE
#define DG_CARD_3_SERIAL_CHANNEL_2_HANDSHAKE 1
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
#ifndef DG_CARD_0_SERIAL_CHANNEL_2_CTS_FLOW
#define DG_CARD_0_SERIAL_CHANNEL_2_CTS_FLOW 0
#endif
#ifndef DG_CARD_1_SERIAL_CHANNEL_2_CTS_FLOW
#define DG_CARD_1_SERIAL_CHANNEL_2_CTS_FLOW 0
#endif
#ifndef DG_CARD_2_SERIAL_CHANNEL_2_CTS_FLOW
#define DG_CARD_2_SERIAL_CHANNEL_2_CTS_FLOW 0
#endif
#ifndef DG_CARD_3_SERIAL_CHANNEL_2_CTS_FLOW
#define DG_CARD_3_SERIAL_CHANNEL_2_CTS_FLOW 0
#endif




#endif /* DG_HDIO_H */
