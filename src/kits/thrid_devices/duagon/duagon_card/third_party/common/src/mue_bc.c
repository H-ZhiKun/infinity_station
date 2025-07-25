/* ==========================================================================
 *
 *  File      : MUE_BC.C
 *
 *  Purpose   : MVB UART Emulation - Bus Controller Structure
 *              - HOST: PC
 *              - OS  : - Windows 9x       (Microsoft Visual C++ V6.0)
 *                      - Windows NT/2K/XP (Microsoft Visual C++ V6.0)
 *              - UART: MVB UART Emulation 'MDFULL'
 *                      accessible over PAR communication channel
 *
 *  Project   : TCN Driver Software
 *              - MVB UART Emulation Protocol (d-000206-nnnnnn)
 *              - TCN Software Architecture   (d-000487-nnnnnn)
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

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>
#include <mue_bc.h>


/* ==========================================================================
 *
 *  Public Variables
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Bus Controller Structure for MVB UART Emulation
 * --------------------------------------------------------------------------
 */
MUE_BUS_CTRL mue_bus_ctrl =                                                 \
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
    (UNSIGNED32)0                                                           \
};
