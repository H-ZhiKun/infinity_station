#ifndef TCN_DEF_H
#define TCN_DEF_H

/* ==========================================================================
 *
 *  File      : TCN_DEF.H
 *
 *  Purpose   : Common Interface Definitions (constants and data types)
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
 *  Pre-processor Definitions:
 *  --------------------------
 *  - TCN_LE      - required for little-endian CPU, e.g. Intel
 *  - __ECOS__    - eCos using GNU GCC
 *  - _MSC_VER    - DOS, Windows using Microsoft Visual C++
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
 *  OS specific Definitions from dg_ue2g
 * --------------------------------------------------------------------------
 */
#include "os_def.h"
#include "osl.h"

/* --------------------------------------------------------------------------
 *  Support for a little-endian CPU, e.g. Intel (PC).
 * --------------------------------------------------------------------------
 */
#if (DG_HOST_SWAP == 0)
#define TCN_LE
#endif 

/* --------------------------------------------------------------------------
 *  MVB interface hardware (MVB UART Emulation 'MDFULL')
 * --------------------------------------------------------------------------
 */
#define MUE_PD_FULL
#define MUE_TEST_MDFL
#ifndef MUELL_MDFL
#define MUELL_MDFL
#endif

/* --------------------------------------------------------------------------
 *  TCN_VERSION_INFORMATION
 * --------------------------------------------------------------------------
 */
#define VERSION_STR_NAME_TCN    "TCN Driver Dxxx"
#define VERSION_STR_IDENTNR_TCN "d-00xxxx-0xxxxx"

/* ==========================================================================
 *
 *  Data Types
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Data types with less than 8-bits
 * --------------------------------------------------------------------------
 */
#ifndef MVB_DEF_H
typedef DG_U8     BOOLEAN1;
typedef DG_U8     ANTIVALENT2;
typedef DG_U8     BCD4;
typedef DG_U8     ENUM4;

/* --------------------------------------------------------------------------
 *  8-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_U8     BITSET8;
typedef DG_U8     WORD8;
typedef DG_U8     ENUM8;
typedef DG_U8     UNSIGNED8;
typedef DG_CHAR8     INTEGER8;
typedef DG_CHAR8     CHARACTER8;

/* --------------------------------------------------------------------------
 *  16-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_U16    BITSET16;
typedef DG_U16    WORD16;
typedef DG_U16    ENUM16;
typedef DG_U16    UNSIGNED16;
typedef DG_S16    INTEGER16;
typedef DG_U16    BIPOLAR2_16;
typedef DG_U16    UNIPOLAR2_16;
typedef DG_U16    BIPOLAR4_16;

/* --------------------------------------------------------------------------
 *  32-bit data types
 * --------------------------------------------------------------------------
 */
typedef float       REAL32;
typedef DG_U32    BITSET32;
typedef DG_U32    WORD32;
typedef DG_U32    ENUM32;
typedef DG_U32    UNSIGNED32;
typedef DG_S32    INTEGER32;

#endif
/* --------------------------------------------------------------------------
 *  64-bit data types
 * --------------------------------------------------------------------------
 */
typedef DG_U64    BITSET64;
typedef DG_U64    WORD64;
typedef DG_U64    ENUM64;
typedef DG_U64    UNSIGNED64;
typedef DG_S64    INTEGER64;

/* --------------------------------------------------------------------------
 *  Structured data types
 * --------------------------------------------------------------------------
 */
#define TCN_SIZEOF_STRUCT_BITSET256 32
typedef struct
{
    BITSET8     byte[32];
}   BITSET256;

#ifndef MVB_DEF_H
#define TCN_SIZEOF_STRUCT_TIMEDATE48 8
typedef struct
{
    UNSIGNED32  seconds;
    UNSIGNED16  ticks;
}   TIMEDATE48;

#define TCN_SIZEOF_STRUCT_STRING32 32
typedef struct
{
    CHARACTER8  character[32];
}   STRING32;
#endif

/* --------------------------------------------------------------------------
 *  Special data type - BITFIELD16
 *  NOTE:
 *  16-bit data type used to build structured types like DS_NAME,
 *  PV_NAME, SV_MVB_DEVICE_STATUS.
 *  This will result in structured types, which size are multiplies
 *  of 16-bits.
 * --------------------------------------------------------------------------
 */
#ifndef TCN_DEF_BITFIELD16
#   define TCN_DEF_BITFIELD16   UNSIGNED16
#endif
typedef TCN_DEF_BITFIELD16      BITFIELD16;


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

#define TCN_DECL_PUBLIC DG_DECL_PUBLIC
#define TCN_DECL_CONST DG_DECL_CONST
#define TCN_DECL_LOCAL  DG_DECL_LOCAL

/* --------------------------------------------------------------------------
 *  Allow creation of procedure-like macros that are a single statement,
 *  and must be followed by a semi-colon.
 * --------------------------------------------------------------------------
 */
#define TCN_MACRO_START         do {
#define TCN_MACRO_END           } while (0)

#define TCN_EMPTY_STATEMENT     TCN_MACRO_START TCN_MACRO_END

#define TCN_UNUSED_PARAMETER(_type_, _name_)    \
    TCN_MACRO_START                             \
        _type_ __tmp1 = (_name_);               \
        _type_ __tmp2 = __tmp1;                 \
        __tmp1 = __tmp2;                        \
    TCN_MACRO_END

/* --------------------------------------------------------------------------
 *  Endian conversion macros
 * --------------------------------------------------------------------------
 */
#define TCN_MACRO_SWAP16(_address_)                                         \
    TCN_MACRO_START                                                         \
        WORD16      _value = *(WORD16*)(_address_);                         \
        WORD8       *_to   = (WORD8*)(_address_);                           \
        WORD8       *_from = (WORD8*)(&_value);                             \
                                                                            \
        _to[0] = _from[1];                                                  \
        _to[1] = _from[0];                                                  \
    TCN_MACRO_END

#define TCN_MACRO_SWAP32(_address_)                                         \
    TCN_MACRO_START                                                         \
        WORD32      _value = *(WORD32*)(_address_);                         \
        WORD8       *_to   = (WORD8*)(_address_);                           \
        WORD8       *_from = (WORD8*)(&_value);                             \
                                                                            \
        _to[0] = _from[3];                                                  \
        _to[1] = _from[2];                                                  \
        _to[2] = _from[1];                                                  \
        _to[3] = _from[0];                                                  \
    TCN_MACRO_END

#define TCN_MACRO_SWAP64(_address_)                                         \
    TCN_MACRO_START                                                         \
        WORD64      _value = *(WORD64*)(_address_);                         \
        WORD8       *_to   = (WORD8*)(_address_);                           \
        WORD8       *_from = (WORD8*)(&_value);                             \
                                                                            \
        _to[0] = _from[7];                                                  \
        _to[1] = _from[6];                                                  \
        _to[2] = _from[5];                                                  \
        _to[3] = _from[4];                                                  \
        _to[4] = _from[3];                                                  \
        _to[5] = _from[2];                                                  \
        _to[6] = _from[1];                                                  \
        _to[7] = _from[0];                                                  \
    TCN_MACRO_END

#ifdef TCN_LE
#   define TCN_MACRO_CONVERT_CPU_TO_BE16(_x_)   TCN_MACRO_SWAP16((_x_))
#   define TCN_MACRO_CONVERT_CPU_TO_BE32(_x_)   TCN_MACRO_SWAP32((_x_))
#   define TCN_MACRO_CONVERT_CPU_TO_BE64(_x_)   TCN_MACRO_SWAP64((_x_))
#   define TCN_MACRO_CONVERT_BE16_TO_CPU(_x_)   TCN_MACRO_SWAP16((_x_))
#   define TCN_MACRO_CONVERT_BE32_TO_CPU(_x_)   TCN_MACRO_SWAP32((_x_))
#   define TCN_MACRO_CONVERT_BE64_TO_CPU(_x_)   TCN_MACRO_SWAP64((_x_))

#   define TCN_MACRO_CONVERT_CPU_TO_LE16(_x_)
#   define TCN_MACRO_CONVERT_CPU_TO_LE32(_x_)
#   define TCN_MACRO_CONVERT_CPU_TO_LE64(_x_)
#   define TCN_MACRO_CONVERT_LE16_TO_CPU(_x_)
#   define TCN_MACRO_CONVERT_LE32_TO_CPU(_x_)
#   define TCN_MACRO_CONVERT_LE64_TO_CPU(_x_)
#else /* #ifdef TCN_LE */
#   define TCN_MACRO_CONVERT_CPU_TO_BE16(_x_)
#   define TCN_MACRO_CONVERT_CPU_TO_BE32(_x_)
#   define TCN_MACRO_CONVERT_CPU_TO_BE64(_x_)
#   define TCN_MACRO_CONVERT_BE16_TO_CPU(_x_)
#   define TCN_MACRO_CONVERT_BE32_TO_CPU(_x_)
#   define TCN_MACRO_CONVERT_BE64_TO_CPU(_x_)

#   define TCN_MACRO_CONVERT_CPU_TO_LE16(_x_)   TCN_MACRO_SWAP16((_x_))
#   define TCN_MACRO_CONVERT_CPU_TO_LE32(_x_)   TCN_MACRO_SWAP32((_x_))
#   define TCN_MACRO_CONVERT_CPU_TO_LE64(_x_)   TCN_MACRO_SWAP64((_x_))
#   define TCN_MACRO_CONVERT_LE16_TO_CPU(_x_)   TCN_MACRO_SWAP16((_x_))
#   define TCN_MACRO_CONVERT_LE32_TO_CPU(_x_)   TCN_MACRO_SWAP32((_x_))
#   define TCN_MACRO_CONVERT_LE64_TO_CPU(_x_)   TCN_MACRO_SWAP64((_x_))
#endif /* #else */

/* --------------------------------------------------------------------------
 *  Miscellaneous - TCN_MACRO_FOREVER
 * --------------------------------------------------------------------------
 */
#define TCN_MACRO_FOREVER   { for(;;) { ; } }

/* --------------------------------------------------------------------------
 *  Miscellaneous - TCN_MACRO_TERMINATE_MAIN (default)
 * --------------------------------------------------------------------------
 */
#define TCN_MACRO_TERMINATE_MAIN(_exit_code_)   return((int)(_exit_code_))

/* --------------------------------------------------------------------------
 *  Miscellaneous - TCN_MACRO_TERMINATE_MAIN (__ECOS_WITH_MICROMONITOR__)
 * --------------------------------------------------------------------------
 */
#ifdef __ECOS_WITH_MICROMONITOR__

TCN_DECL_PUBLIC void mon_appexit(int exitval);

#undef  TCN_MACRO_TERMINATE_MAIN
#define TCN_MACRO_TERMINATE_MAIN(_exit_code_)                               \
            mon_appexit((int)(_exit_code_))

#endif /* #ifdef __ECOS_WITH_MICROMONITOR__ */

/* --------------------------------------------------------------------------
 *  Miscellaneous - TCN_MACRO_TERMINATE_MAIN (_MSC_VER)
 * --------------------------------------------------------------------------
 */
#ifdef _MSC_VER
#ifdef _DEBUG

#undef  TCN_MACRO_TERMINATE_MAIN
#define TCN_MACRO_TERMINATE_MAIN(_exit_code_)                               \
            TCN_MACRO_FOREVER

#endif /* #ifdef _DEBUG */
#endif /* #ifdef _MSC_VER */


#define TCN_OSL_PRINTF printf

#define DG_DEFAULT_IDX 0

#endif /* #ifndef TCN_DEF_H */
