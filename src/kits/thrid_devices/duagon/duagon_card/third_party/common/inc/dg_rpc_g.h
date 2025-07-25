/* ==========================================================================
 *
 *  File      : DG_IP_G.H
 *
 *  Purpose   : Common part for RPC (Host/Device)
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

#ifndef DG_RPC_G_H
#define DG_RPC_G_H

/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

#include "os_def.h"

/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Macros for declaration of variables and procedures.
 *  NOTE:
 *  extern "C" is used in mixed C/C++ headers to force C linkage on an
 *  external definition.
 * --------------------------------------------------------------------------
 */
#define RPC_DECL_PUBLIC DG_DECL_PUBLIC
#define RPC_DECL_LOCAL DG_DECL_LOCAL
#define RPC_DECL_CONST DG_DECL_CONST


typedef struct
{
    DG_U32   nb_entry;
    void       **p_data;
    DG_U32   *nb_bytes;
}   CCH_CMD_BLOCK_STRUCT;

#define RPC_PROTOCOL_DATA CCH_CMD_BLOCK_STRUCT

#define MAX_RECV_SIZE 22
#define MAX_SEND_SIZE 22
#define MAX_ARRAY_LENGTH 20

/* --------------------------------------------------------------------------
 *  8-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_CHAR8 RPC_CHAR8;
typedef DG_S8 RPC_S8;
typedef DG_U8 RPC_U8;
typedef DG_BOOL RPC_BOOL;


/* --------------------------------------------------------------------------
 *  16-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_U16 RPC_U16;
typedef DG_S16 RPC_S16;

/* --------------------------------------------------------------------------
 *  32-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_U32 RPC_U32;
typedef DG_S32 RPC_S32;



enum RPC_DATA_TYPES {
	RPC_ARRAY8 = 1,
	RPC_TEXT = 2,
	RPC_8 = 3,
	RPC_16 = 4,
	RPC_32 = 5,
	RPC_ARRAY32 = 6,
	RPC_RETURN_ARRAY8 = 20,
	RPC_RETURN_8 = 21,
	RPC_RETURN_16 = 22,
	RPC_RETURN_32 = 23,
	RPC_IO_32 = 40,
	RPC_IO_16 = 41,
	RPC_IO_8 = 42,
	RPC_END = 127/* always last elementin enum */
};

#endif /* DG_RPC_G_H */
