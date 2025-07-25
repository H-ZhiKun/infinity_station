/* ==========================================================================
 *
 *  File      : DG_RPC.H
 *
 *  Purpose   : Remote procedure call RPC for UART emulation 2G (Host side)
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

#ifndef DG_RPC_H
#define DG_RPC_H


/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

#include "dg_rpc_g.h"
#include "dg_rtm.h"
#include "dg_error.h"
#include "dg_hdio.h"



/* --------------------------------------------------------------------------
 *  special data types
 * --------------------------------------------------------------------------
 */

/*
 * Send function definition used by rpc. This function will transfer data
 * from host to device
 */
typedef RPC_S32 (*DuagonSendFunc)
        (RPC_U8, RTM_HANDLE_P handle, RPC_PROTOCOL_DATA *send, RPC_PROTOCOL_DATA *recv);


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Endian conversion macros
 * --------------------------------------------------------------------------
 */

#define DG_MACRO_START     {
#define DG_MACRO_END       }


#define DG_MACRODEF_SWAP16(_address_)                                        \
   DG_MACRO_START                                                         \
        DG_U16      _value = *(DG_U16*)(_address_);                     \
        DG_U8       *_to   = (DG_U8*)(_address_);                       \
        DG_U8       *_from = (DG_U8*)(&_value);                         \
                                                                            \
        _to[0] = _from[1];                                                  \
        _to[1] = _from[0];                                                  \
    DG_MACRO_END

#define DG_MACRODEF_SWAP32(_address_)                                        \
    DG_MACRO_START                                                        \
        DG_U32      _value = *(DG_U32*)(_address_);                     \
        DG_U8       *_to   = (DG_U8*)(_address_);                       \
        DG_U8       *_from = (DG_U8*)(&_value);                         \
                                                                            \
        _to[0] = _from[3];                                                  \
        _to[1] = _from[2];                                                  \
        _to[2] = _from[1];                                                  \
        _to[3] = _from[0];                                                  \
    DG_MACRO_END

#define DG_MACRODEF_SWAP64(_address_)                                        \
    DG_MACRO_START                                                        \
        DG_U64      _value = *(DG_U64*)(_address_);                     \
        DG_U8       *_to   = (DG_U8*)(_address_);                       \
        DG_U8       *_from = (DG_U8*)(&_value);                         \
                                                                            \
        _to[0] = _from[7];                                                  \
        _to[1] = _from[6];                                                  \
        _to[2] = _from[5];                                                  \
        _to[3] = _from[4];                                                  \
        _to[4] = _from[3];                                                  \
        _to[5] = _from[2];                                                  \
        _to[6] = _from[1];                                                  \
        _to[7] = _from[0];                                                  \
    DG_MACRO_END

#if (DG_HOST_SWAP == 1)

#define DG_MACRO_SWAP16(_address_) DG_MACRODEF_SWAP16(_address_)
#define DG_MACRO_SWAP32(_address_) DG_MACRODEF_SWAP32(_address_)
#define DG_MACRO_SWAP64(_address_) DG_MACRODEF_SWAP64(_address_)


#else

#define DG_MACRO_SWAP16(_address_)                                        \
    DG_MACRO_START                                                        \
    DG_MACRO_END

#define DG_MACRO_SWAP32(_address_)                                        \
    DG_MACRO_START                                                        \
    DG_MACRO_END

#define DG_MACRO_SWAP64(_address_)                                        \
    DG_MACRO_START                                                        \
    DG_MACRO_END

#endif

/*
 * if RPC_ARGS_CHECK_ENABLE every call will first check if arguments on
 * device are defined as on host, this is used for debug
 */
/*#define RPC_ARGS_CHECK_ENABLE*/
/*
 * IF RPC_PERF_CHECK_ENABLE every call will be measured on its performance (windows only)
 */
#define RPC_PERF_CHECK_ENABLE

/*
 * swap 2 Byte value if necessary (LSB)
 */
#define RPC_MACRO_SWAP16 DG_MACRO_SWAP16
/*
 * swap 4 Byte value if necessary (LSB)
 */
#define RPC_MACRO_SWAP32 DG_MACRO_SWAP32

/*
 * Callback function for calling at begin and end of rpc call
 */
typedef void (*DuagonBoundaryFunc)(RPC_U8 protocol, RPC_U8 command, DG_U8 card_index);

/* --------------------------------------------------------------------------
 * Procedure :  dg_rpc_set_boundary_callbacks
 *
 * Purpose :    allows to set callbacks for begin and end of rpc calls
 *
 * Syntax :     RPC_DECL_PUBLIC
 *              void
 *              dg_rpc_set_boundary_callbacks
 *              (
 *                  DuagonBoundaryFunc in,
 *                  DuagonBoundaryFunc out
 *              );
 *
 * Input :      in      Function to be called at begin of rpc call
 *              out     Function to be called at end of rpc call
 * Return :     --
 * Errors :     --
 *
 * Remarks :    local function
 * --------------------------------------------------------------------------
 */
RPC_DECL_PUBLIC
void
dg_rpc_set_boundary_callbacks
(
        DuagonBoundaryFunc in,
        DuagonBoundaryFunc out,
        DG_U8 card_index
);

/* --------------------------------------------------------------------------
 * Procedure :  dg_rpc_send_func
 *
 * Purpose :    convert values between host and network byte order
 *
 * Syntax :     IP_DECL_PUBLIC
 *              void
 *              dg_rpc_send_func
 *              (
 *                  DuagonSendFunc func
 *              )
 *
 * Input :      func        Function to send data to duagon device
 *              card_index  To specify which card in multi-card system
 *                          (give 0 for only 1 card)
 * Return :     --
 * Errors :     --
 *
 * Remarks :    local function
 * --------------------------------------------------------------------------
 */
RPC_DECL_PUBLIC
void
dg_rpc_send_func
(
    DuagonSendFunc func,
    DG_U8 card_index
);


/* --------------------------------------------------------------------------
 * Procedure :  dg_rpc
 *
 * Purpose :    convert values between host and network byte order
 *
 * Syntax :     IP_DECL_PUBLIC
 *              void
 *              dg_rpc
 *              (
 *                  RPC_U8 protocol,
 *                  RPC_U8 command,
 *                  ...
 *              )
 *
 * Input :      protocol in
 *                  unique id for command family
 *              command in
 *                  id, unique in protocol for command
 *              ...
 *                  varags depending on command
 *                  all variable are transfered as pointer
 *                  RPC_ARRAY8          buffer pointer to len
 *                  RPC_TEXT            buffer (0 termination string)
 *                  RPC_8               pointer to 8 bit value
 *                  RPC_16              pointer to 16 bit value
 *                  RPC_32              pointer to 32 bit value
 *                  RPC_ARRAY32         buffer pointer to len
 *                  RPC_RETURN_ARRAY8   pointer to return buffer
 *                  RPC_RETURN_8        pointer to return 8 bit value
 *                  RPC_RETURN_16       pointer to return 16 bit value
 *                  RPC_RETURN_32       pointer to return 32 bit value
 *                  RPC_IO_32           pointer to 32 bit input/return value
 *                  RPC_END             marker for last argument
 *
 * Return :     if ok 0, otherwise -1
 * Errors :     --
 *
 * Remarks :    local function
 *
 * --------------------------------------------------------------------------
 */
RPC_DECL_PUBLIC
DG_RESULT
dg_rpc
(
    RPC_U8 protocol,
    RPC_U8 command,
    RTM_HANDLE_P handle,
    ...
);


/*


 * Remark:
 *
 * This is the asynchronous version of dg_rpc. For queuing an
 * asynchronous request, execute the method with *handle == 0. The method
 * returns a real handle in the same variable. Then, poll for the result in
 * a loop using the received handle until *handle == 0 again. Then the result
 * is returned.
 *
 * handle in/out
 *  NULL for synchronous requests, not NULL for asynchronous
 *  requests. If not null and *handle == 0, posts a new
 *  request. The device will return a handle in the same
 *  variable that is used to poll for the result until the
 *  device returns *handle == 0.
 * */











#endif /* DG_RPC_H */
