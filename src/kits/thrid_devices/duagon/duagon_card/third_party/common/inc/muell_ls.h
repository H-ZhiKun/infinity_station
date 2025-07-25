#ifndef MUELL_LS_H
#define MUELL_LS_H

/* ==========================================================================
 *
 *  File      : MUELL_LS.H
 *
 *  Purpose   : Bus Controller Link Layer for MUE (common) - 
 *                  Link Layer Bus Controller Interface for Supervision
 *
 *  Project   : General TCN Driver Software
 *              - TCN Software Architecture (d-000487-nnnnnn)
 *
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
#include <tcn_ls.h>


/* ==========================================================================
 *
 *  Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Miscellaneous
 * --------------------------------------------------------------------------
 */
#define MUELL_LS_DEVICE_NUMBER_MAX          4096
TCN_DECL_PUBLIC TCN_DECL_CONST UNSIGNED16   muell_ls_device_number_max;


/* ==========================================================================
 *
 *  Bus Controller Interface
 *
 * ==========================================================================
 */
#ifndef DG_MULTI_CARD

/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_init (of procedure type 'LS_INIT')
 *
 *  Purpose   : Initialises a link layer as a whole.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              muell_ls_init
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 *              - This procedure has to be called at system
 *                initialisation before calling any other 'xxx_ls_xxx',
 *                'xxx_lp_xxx' or 'xxx_lm_xxx' procedure referencing
 *                the same link layer (parameter 'p_bus_ctrl').
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
muell_ls_init
(
    void        *p_bus_ctrl
);


/* --------------------------------------------------------------------------
 *  Procedure : muell_ls_service_handler
 *              (of procedure type 'LS_SERVICE_HANDLER')
 *
 *  Purpose   : Handles link layer specific services.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              LS_RESULT
 *              muell_ls_service_handler
 *              (
 *                  void        *p_bus_ctrl,
 *                  ENUM8       service,
 *                  void        *p_parameter
 *              );
 *
 *  Input     : p_bus_ctrl  - pointer to bus controller specific values
 *              service     - link layer service
 *
 *  In-/Output: p_parameter - pointer to a memory buffer which contains
 *                            service specific values
 *
 *  Return    : result code; any LS_RESULT
 *
 *  Remarks   : - A link layer is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
LS_RESULT
muell_ls_service_handler
(
    void        *p_bus_ctrl,
    ENUM8       service,
    void        *p_parameter
);

#endif /* DG_MULTI_CARD */

/* --------------------------------------------------------------------------
 * Versions with card index (for descriptions see above)
 * --------------------------------------------------------------------------
 */

TCN_DECL_PUBLIC
LS_RESULT
muell_ls_init_idx
(
    void        *p_bus_ctrl,
    DG_U8       card_index
);

TCN_DECL_PUBLIC
LS_RESULT
muell_ls_service_handler_idx
(
    void        *p_bus_ctrl,
    ENUM8       service,
    void        *p_parameter,
    DG_U8       card_index
);
/*
* --------------------------------------------------------------------------
*/



#endif /* #ifndef MUELL_LS_H */
