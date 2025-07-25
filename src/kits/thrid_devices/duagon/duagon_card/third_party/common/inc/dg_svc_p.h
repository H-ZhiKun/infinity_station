/* ==========================================================================
 *
 *  File      : DG_SVC_P.H
 *
 *  Purpose   : Service Protocol definitions/functions that are not part of
 *              the API
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 *  Remarks   : !!! DO NOT CHANGE THIS FILE !!!
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon AG, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */

#ifndef DG_SRV_P_H
#define DG_SRV_P_H


#include "os_def.h"
#include "dg_rtm.h"


/* ==========================================================================
 *
 *  Protocol definitions 
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Protocol ID
 * --------------------------------------------------------------------------
 */
#define SVC_PROTOCOL_ID 0x0F

/* --------------------------------------------------------------------------
 *  Command IDs
 * --------------------------------------------------------------------------
 */
enum {
	CMD_SVC_GET = 0,		/* svc_get */
	CMD_SVC_SET = 1,		/* svc_set */
	CMD_SVC_LOOPBACK = 2,	/* svc_loopback */
	CMD_SVC_INIT = 3,
	CMD_SVC_LAST			/* mark last element */
};

#define SVC_DATA_SIZE_MAX   65536

/* ==========================================================================
 *
 *  Definition of data types and keywords
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  8-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_S8            SVC_S8;
typedef DG_U8            SVC_U8;
typedef DG_CHAR8         SVC_CHAR8;

/* --------------------------------------------------------------------------
 *  16-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_U16           SVC_U16;
typedef DG_S16           SVC_S16;

/* --------------------------------------------------------------------------
 *  32-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_S32           SVC_S32;
typedef DG_U32           SVC_U32;

/* --------------------------------------------------------------------------
 *  Misc keyword definitions
 * --------------------------------------------------------------------------
 */
#define SVC_DECL_PUBLIC  DG_DECL_PUBLIC
#define SVC_DECL_LOCAL   DG_DECL_LOCAL
#define SVC_DECL_CONST   DG_DECL_CONST

#ifndef NULL
 #define NULL (void*)0
#endif


/* ==========================================================================
 *
 *  Public procedure interface (debug functions only)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 * Procedure :  svc_loopback
 *
 * Purpose   :  Test a loopback through RPC layer
 *
 * Syntax :
 *
 * Input:       outData to be send
 * 				outLen len of data
 * 				backData loop back data
 *              backLen size of buffer for back data
 * Return: 		len of copied data or -1 in case of a error
 * Remark:
 * --------------------------------------------------------------------------
 */
SVC_DECL_PUBLIC
SVC_S32
svc_loopback
(
	SVC_DECL_CONST SVC_CHAR8 *outData,
	SVC_S32                  outLen,
	SVC_CHAR8                *backData,
	SVC_S32                  backLen,
	SVC_S32                  *svc_errno,
	RTM_HANDLE_P             rtHandle
);

#endif /* DG_SRV_P_H */
