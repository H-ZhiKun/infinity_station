/* ==========================================================================
 *
 *  File      : TCN_BLS.C
 *
 *  Purpose   : Bus Controller Link Layer Selector
 *              - HOST: PC
 *              - OS  : - Windows 9x       (Microsoft Visual C++ V6.0)
 *                      - Windows NT/2K/XP (Microsoft Visual C++ V6.0)
 *              - UART: MVB UART Emulation 'MDFULL'
 *                      accessible over PAR communication channel
 *
 *  Project   : TCN Driver Software for D013
 *              - MVB UART Emulation Protocol (d-000206-nnnnnn)
 *              - TCN Software Architecture   (d-000487-nnnnnn)
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon GmbH, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Project specific Definitions used for Conditional Compiling
 *
 * ==========================================================================
 */
#ifdef TCN_PRJ
#   include <tcn_prj.h>
#endif


/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>
#include <tcn_bls.h>

/* --------------------------------------------------------------------------
 *  TCN Bus Controller Link Layer - MDFULL
 * --------------------------------------------------------------------------
 */
#include <mdfl_bc.h>
#include <mdfl_ls.h>
#include <mdfl_lp.h>
#ifdef TCN_MD
#   include <mdfl_lm.h>
#endif /* #ifdef TCN_MD */


/* ==========================================================================
 *
 *  MUE Link Layer 'MDFULL'
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bus Controller Structure for MUE Link Layer 'MDFULL'
 * --------------------------------------------------------------------------
 */
MDFULL_BUS_CTRL bc_mdfl =                                                   \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  UART (static values)                                                \
     *                                                                      \
     *  NOTE:                                                               \
     *  Values not used. See project specific implementation 'mue_acc'.     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* address of UART register RBR/THR                                 */  \
    (WORD32)0,                                                              \
                                                                            \
    /* address of UART register LSR                                     */  \
    (WORD32)0,                                                              \
                                                                            \
    /* size of a UART register (number of bytes)                        */  \
    (UNSIGNED32)0,                                                          \
                                                                            \
    /* timeout by loop (~10ms for MDFULL)                               */  \
    (UNSIGNED32)0,                                                          \
                                                                            \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  LS (static values)                                                  \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* device status capability                                         */  \
    /* - FALSE - no   DS capability                                     */  \
    /* - TRUE  - with DS capability                                     */  \
    TRUE,                                                                   \
                                                                            \
    /* bus administrator capability                                     */  \
    /* - FALSE - no   BA capability                                     */  \
    /* - TRUE  - with BA capability                                     */  \
    TRUE,                                                                   \
                                                                            \
    /* max. number of entries in the BA list                            */  \
    8192                                                                    \
                                                                            \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  dynamic values (ignore)                                             \
     *  NOTE:                                                               \
     *  Dynamic values will be set to 0 by initialisation of the            \
     *  variable and later modified by the software.                        \
     * ------------------------------------------------------------------   \
     */                                                                     \
};

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Supervision (SV)
 *  (MUE Link Layer 'MDFULL')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_SV_MDFL /* (of type 'BLS_DESC_SV')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LSBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LS_INIT'                                    */  \
    mdfull_ls_init,                                                         \
    /* a procedure of type 'LS_SERVICE_HANDLER'                         */  \
    mdfull_ls_service_handler                                               \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Process Data (PD)
 *  (MUE Link Layer 'MDFULL')
 * --------------------------------------------------------------------------
 */
#define BLS_DESC_PD_MDFL /* (of type 'BLS_DESC_PD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LPBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LP_INIT'                                    */  \
    mdfull_lp_init,                                                         \
    /* a procedure of type 'LP_MANAGE'                                  */  \
    mdfull_lp_manage,                                                       \
    /* a procedure of type 'LP_PUT_DATASET'                             */  \
    mdfull_lp_put_dataset,                                                  \
    /* a procedure of type 'LP_GET_DATASET'                             */  \
    mdfull_lp_get_dataset                                                   \
}

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Service Descriptor for Message Data (MD)
 *  (MUE Link Layer 'MDFULL')
 * --------------------------------------------------------------------------
 */
#ifdef TCN_MD
#define BLS_DESC_MD_MDFL /* (of type 'BLS_DESC_MD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LMBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_INIT'                                    */  \
    mdfull_lm_init,                                                         \
    /* a procedure of type 'LM_GET_DEV_ADDRESS'                         */  \
    mdfull_lm_get_dev_address,                                              \
    /* a procedure of type 'LM_GET_STATUS'                              */  \
    mdfull_lm_get_status,                                                   \
    /* a procedure of type 'LM_SEND_QUEUE_FLUSH'                        */  \
    mdfull_lm_send_queue_flush,                                             \
    /* a procedure of type 'LM_SEND_REQUEST'                            */  \
    mdfull_lm_send_request,                                                 \
    /* a procedure of type 'LM_RECEIVE_POLL'                            */  \
    mdfull_lm_receive_poll,                                                 \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  subscribed procedures to the LMBI                                   \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_RECEIVE_INDICATE'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_PACK'                                */  \
    NULL,                                                                   \
    /* identifies the packet pool used by the link layer                */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_CONFIRM'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_STATUS_INDICATE'                         */  \
    NULL                                                                    \
}
#else
#define BLS_DESC_MD_MDFL /* (of type 'BLS_DESC_MD')                     */  \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  LMBI procedures                                                     \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_INIT'                                    */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_DEV_ADDRESS'                         */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_STATUS'                              */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_QUEUE_FLUSH'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_REQUEST'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_RECEIVE_POLL'                            */  \
    NULL,                                                                   \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  subscribed procedures to the LMBI                                   \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* a procedure of type 'LM_RECEIVE_INDICATE'                        */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_GET_PACK'                                */  \
    NULL,                                                                   \
    /* identifies the packet pool used by the link layer                */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_SEND_CONFIRM'                            */  \
    NULL,                                                                   \
    /* a procedure of type 'LM_STATUS_INDICATE'                         */  \
    NULL                                                                    \
}
#endif /* #ifdef TCN_MD */

/* --------------------------------------------------------------------------
 *  Bus Controller Link Layer Descriptor for MUE Link Layer 'MDFULL'
 * --------------------------------------------------------------------------
 */
BLS_DESC bls_desc_0 =                                                       \
{                                                                           \
    /* ------------------------------------------------------------------   \
     *  bus controller description                                          \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* pointer to bus controller specific values                        */  \
    &bc_mdfl,                                                               \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  link description                                                    \
     * ------------------------------------------------------------------   \
     */                                                                     \
    /* type of the link (any SV_LINK_TYPE)                              */  \
    SV_LINK_TYPE_MVB,                                                       \
    /* name of the link                                                 */  \
    /* STRING32 (should be null terminated)                             */  \
    /* <-- 10 --><-- 20 --><-- 30 -->32                                 */  \
    /* 12345678901234567890123456789012                                 */  \
    { "Duagon D015-MDFULL" },                                               \
                                                                            \
    /* ------------------------------------------------------------------   \
     *  service descriptors                                                 \
     * ------------------------------------------------------------------   \
     */                                                                     \
    BLS_DESC_SV_MDFL,                                                       \
    BLS_DESC_PD_MDFL,                                                       \
    BLS_DESC_MD_MDFL                                                        \
};


/* ==========================================================================
 *
 *  Public Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Public Variable   : bls_desc_table
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
BLS_DESC *bls_desc_table[BLS_MAX_LINKS] =                                   \
{                                                                           \
    &bls_desc_0,                                        /* link_id =  0 */  \
    NULL,                                               /* link_id =  1 */  \
    NULL,                                               /* link_id =  2 */  \
    NULL,                                               /* link_id =  3 */  \
    NULL,                                               /* link_id =  4 */  \
    NULL,                                               /* link_id =  5 */  \
    NULL,                                               /* link_id =  6 */  \
    NULL,                                               /* link_id =  7 */  \
    NULL,                                               /* link_id =  8 */  \
    NULL,                                               /* link_id =  9 */  \
    NULL,                                               /* link_id = 10 */  \
    NULL,                                               /* link_id = 11 */  \
    NULL,                                               /* link_id = 12 */  \
    NULL,                                               /* link_id = 13 */  \
    NULL,                                               /* link_id = 14 */  \
    NULL                                                /* link_id = 15 */  \
};
