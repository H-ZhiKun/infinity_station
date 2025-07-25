/* ==========================================================================
 *
 *  File      : TCN_LP.C
 *
 *  Purpose   : Link Layer Interface for Process Data
 *
 *  Project   : General TCN Driver Software
 *              - TCN Software Architecture (d-000487-nnnnnn)
 *
 *  Version   : d-000543-nnnnnn
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
#include <tcn_pd.h>
#include <tcn_lp.h>


/******************************************************************************/
/*   For multi-card  related definitions                                      */
/******************************************************************************/
#include "dg_hdio.h"


/* ==========================================================================
 *
 *  General Constants and Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous Constants
 * --------------------------------------------------------------------------
 */
TCN_DECL_CONST UNSIGNED16   lp_max_traffic_stores = LP_MAX_TRAFFIC_STORES;

TCN_DECL_CONST UNSIGNED16   pd_prt_port_number_max = PD_PRT_PORT_NUMBER_MAX;


/* ==========================================================================
 *
 *  Common Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : lp_init
 *
 *  Purpose   : Initialises a process data link layer.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              lp_init
 *              (
 *                  ENUM8       ts_id,
 *                  UNSIGNED16  fsi
 *              );
 *
 *  Input     : ts_id - traffic store identifier (0..15)
 *              fsi   - freshness supervision interval (value range depends
 *                      on the bus controller)
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 *              - This procedure has to be called at system initialisation
 *                before calling any other 'lp_xxx' procedure referencing
 *                the same traffic store (parameter 'ts_id').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
lp_init
(
    ENUM8       ts_id,
    UNSIGNED16  fsi
)
{
    return lp_init_tcn_idx(ts_id, fsi, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
LP_RESULT
lp_init_tcn_idx
(
    ENUM8       ts_id,
    UNSIGNED16  fsi,
    DG_U8 card_index
)
{
    LP_RESULT   lp_result = LP_OK;

#ifdef TCN_CHECK_PARAMETER
    if (ts_id >= lp_max_traffic_stores)
    {
        lp_result = LP_RANGE;
    } /* if (ts_id >= lp_max_traffic_stores) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (LP_OK == lp_result)
    {
        if (NULL == dg_card_config[card_index].tcbs.bls_descr_table[ts_id])
        {
            lp_result = LP_UNKNOW_TS;
        } /* if (NULL == bls_descr_table[ts_id]) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        if (NULL == dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->desc_pd.lp_init)
        {
            lp_result = LP_UNKNOW_TS;
        } /* if (NULL == bls_descr_table[ts_id]->desc_pd.lp_init) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        lp_result =                                 \
            dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->desc_pd.lp_init  \
            (                                       \
                dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->p_bus_ctrl,  \
                fsi,                                 \
                card_index
            );
    } /* if (LP_OK == lp_result) */

    return(lp_result);

} /* lp_init */


/* --------------------------------------------------------------------------
 *  Procedure : lp_manage
 *
 *  Purpose   : Manages a port located in a traffic store.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              lp_manage
 *              (
 *                  ENUM8       ts_id,
 *                  WORD16      port_address,
 *                  ENUM16      command,
 *                  PD_PRT_ATTR *prt_attr
 *              );
 *
 *  Input     : ts_id        - traffic store identifier (0..15)
 *              port_address - port address (0..4096)
 *              command      - port manage command; any PD_PRT_CMD
 *
 *  In-/Output: prt_attr     - pointer to a memory buffer which contains
 *                             the port attributes (memory buffer of
 *                             structured type 'PD_PRT_ATTR')
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
lp_manage
(
    ENUM8       ts_id,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr
)
{
    return lp_manage_idx(ts_id, port_address, command, prt_attr, DG_DEFAULT_IDX);
}

TCN_DECL_PUBLIC
LP_RESULT
lp_manage_idx
(
    ENUM8       ts_id,
    WORD16      port_address,
    ENUM16      command,
    PD_PRT_ATTR *prt_attr,
    DG_U8       card_index
)
{
    LP_RESULT   lp_result = LP_OK;

#ifdef TCN_CHECK_PARAMETER
    if (ts_id >= lp_max_traffic_stores)
    {
        lp_result = LP_RANGE;
    } /* if (ts_id >= lp_max_traffic_stores) */

    if (LP_OK == lp_result)
    {
        if (port_address >= pd_prt_port_number_max)
        {
            lp_result = LP_RANGE;
        } /* if (port_address >= pd_prt_port_number_max) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        if ((PD_PRT_CMD_CONFIG != command) && \
            (PD_PRT_CMD_DELETE != command) && \
            (PD_PRT_CMD_MODIFY != command) && \
            (PD_PRT_CMD_STATUS != command))
        {
            lp_result = LP_ERROR;
        } /* if ((PD_PRT_CMD_CONFIG != command) && ...) */
    } /* if (LP_OK == lp_result) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (LP_OK == lp_result)
    {
        if (NULL == dg_card_config[card_index].tcbs.bls_descr_table[ts_id])
        {
            lp_result = LP_UNKNOW_TS;
        } /* if (NULL == bls_descr_table[ts_id]) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        if (NULL == dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->desc_pd.lp_manage)
        {
            lp_result = LP_UNKNOW_TS;
        } /* if (NULL == bls_descr_table[ts_id]->desc_pd.lp_manage) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        LP_MANAGE l_lp_manage = (LP_MANAGE)(dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->desc_pd.lp_manage);
        lp_result =                                     \
            l_lp_manage \
            (                                           \
                dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->p_bus_ctrl,      \
                port_address,                           \
                command,                                \
                prt_attr,                                \
                card_index
            );
    } /* if (LP_OK == lp_result) */

    return(lp_result);

} /* lp_port_manage */


/* --------------------------------------------------------------------------
 *  Procedure : lp_put_dataset
 *
 *  Purpose   : Copies a dataset from the application to a port.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              lp_put_dataset
 *              (
 *                  ENUM8       ts_id,
 *                  WORD16      port_address,
 *                  void        *p_value
 *              );
 *
 *  Input     : ts_id        - traffic store identifier (0..15)
 *              port_address - port address (0..4096)
 *              p_value      - pointer to a memory buffer of
 *                             the application where the dataset
 *                             is copied from
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
lp_put_dataset
(
    ENUM8       ts_id,
    WORD16      port_address,
    void        *p_value
)
{
    return lp_put_dataset_tcn_idx(ts_id, port_address, p_value, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
LP_RESULT
lp_put_dataset_tcn_idx
(
    ENUM8       ts_id,
    WORD16      port_address,
    void        *p_value,
    DG_U8       card_index
)
{
    LP_RESULT   lp_result = LP_OK;

#ifdef TCN_CHECK_PARAMETER
    if (ts_id >= lp_max_traffic_stores)
    {
        lp_result = LP_RANGE;
    } /* if (ts_id >= lp_max_traffic_stores) */

    if (LP_OK == lp_result)
    {
        if (port_address >= pd_prt_port_number_max)
        {
            lp_result = LP_RANGE;
        } /* if (port_address >= pd_prt_port_number_max) */
    } /* if (LP_OK == lp_result) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (LP_OK == lp_result)
    {
        if (NULL == dg_card_config[card_index].tcbs.bls_descr_table[ts_id])
        {
            lp_result = LP_UNKNOW_TS;
        } /* if (NULL == bls_descr_table[ts_id]) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        if (NULL == dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->desc_pd.lp_put_dataset)
        {
            lp_result = LP_UNKNOW_TS;
        } /* if (NULL == bls_descr_table[ts_id]->desc_pd.lp_put_dataset) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        lp_result = \
            dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->desc_pd.lp_put_dataset \
            ( \
                dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->p_bus_ctrl, \
                port_address, \
                p_value, \
                card_index \
            );
    } /* if (LP_OK == lp_result) */

    return(lp_result);

} /* lp_put_dataset */


/* --------------------------------------------------------------------------
 *  Procedure : lp_get_dataset
 *
 *  Purpose   : Copies a dataset from a port to the application.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LP_RESULT
 *              lp_get_dataset
 *              (
 *                  ENUM8       ts_id,
 *                  WORD16      port_address,
 *                  void        *p_value,
 *                  void        *p_fresh
 *              );
 *
 *  Input     : ts_id        - traffic store identifier (0..15)
 *              port_address - port address (0..4096)
 *
 *  Return    : result code; any LP_RESULT
 *
 *  Output    : p_value      - pointer to a memory buffer of
 *                             the application where the dataset
 *                             is copied to
 *              p_fresh      - pointer to a memory buffer of
 *                             the application where the freshness timer
 *                             is copied to
 *
 *  Remarks   : - A traffic store is identified by 'ts_id'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LP_RESULT
lp_get_dataset
(
    ENUM8       ts_id,
    WORD16      port_address,
    void        *p_value,
    void        *p_fresh
)
{
    return lp_get_dataset_tcn_idx(ts_id, port_address, p_value, p_fresh, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
LP_RESULT
lp_get_dataset_tcn_idx
(
    ENUM8       ts_id,
    WORD16      port_address,
    void        *p_value,
    void        *p_fresh,
    DG_U8       card_index
)
{
    LP_RESULT   lp_result = LP_OK;

#ifdef TCN_CHECK_PARAMETER
    if (ts_id >= lp_max_traffic_stores)
    {
        lp_result = LP_RANGE;
    } /* if (ts_id >= lp_max_traffic_stores) */

    if (LP_OK == lp_result)
    {
        if (port_address >= pd_prt_port_number_max)
        {
            lp_result = LP_RANGE;
        } /* if (port_address >= pd_prt_port_number_max) */
    } /* if (LP_OK == lp_result) */
#endif /* #ifdef TCN_CHECK_PARAMETER */

    if (LP_OK == lp_result)
    {
        if (NULL == dg_card_config[card_index].tcbs.bls_descr_table[ts_id])
        {
            lp_result = LP_UNKNOW_TS;
        } /* if (NULL == bls_descr_table[ts_id]) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        if (NULL == dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->desc_pd.lp_get_dataset)
        {
            lp_result = LP_UNKNOW_TS;
        } /* if (NULL == bls_descr_table[ts_id]->desc_pd.lp_get_dataset) */
    } /* if (LP_OK == lp_result) */

    if (LP_OK == lp_result)
    {
        lp_result =                                         \
            dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->desc_pd.lp_get_dataset   \
            (                                               \
                dg_card_config[card_index].tcbs.bls_descr_table[ts_id]->p_bus_ctrl,          \
                port_address,                               \
                p_value,                                    \
                p_fresh,                                    \
                card_index \
            );
    } /* if (LP_OK == lp_result) */

    return(lp_result);

} /* lp_get_dataset */
