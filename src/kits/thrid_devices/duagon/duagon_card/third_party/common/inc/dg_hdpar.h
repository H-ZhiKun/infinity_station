/* ==========================================================================
 *
 *  File      : DG_HDPAR.H
 *
 *  Purpose   : Duagon Host Device - Parallel Interface
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

#ifndef DG_HDPAR_H
#define DG_HDPAR_H

#include "dg_hdio.h"


DG_DECL_PUBLIC
void
osl_par_hal_write8
(
        REG_SIZE* addr,
        DG_U8 val
);

DG_DECL_PUBLIC
void
osl_par_hal_write32
(
        REG_SIZE* addr,
        DG_U32 val
);

DG_DECL_PUBLIC
DG_U8
osl_par_hal_read8
(
        REG_SIZE* addr
);

DG_DECL_PUBLIC
DG_U32
osl_par_hal_read32
(
        REG_SIZE* addr
);

/* initializes the par driver (opens the par device) */
DG_DECL_PUBLIC
DG_RESULT
osl_par_hal_install
(
        DG_U32 *addr_list,
        DG_U8 addr_list_len,
        DG_U8 card_index
);

DG_DECL_PUBLIC
DG_RESULT
osl_par_hal_init_channel
(
        DG_HDREG_CONFIG* config
);

#ifdef TESTBOX_MASTER
DG_DECL_PUBLIC
void
osl_par_hal_enable_dut_interface
(
        DG_BOOL enable
);
#endif

#endif /* DG_HDPAR_H */
