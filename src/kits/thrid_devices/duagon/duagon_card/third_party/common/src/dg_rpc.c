/* ==========================================================================
 *
 *  File      : DG_RPC.C
 *
 *  Purpose   : Remote procedure call RPC for UART emulation 2G (Host side)
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
 *  Include Files
 *
 * ==========================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "dg_hdc.h"
#include "dg_rpc.h"
#include "dg_rtm.h"
#include "dg_error.h"
#include "osl.h"

#if (1 == RPC_DEBUG)
#define rpc_debug_printf osl_printf
#else
#define rpc_debug_printf dummy_printf
DG_DECL_LOCAL void dummy_printf(DG_DECL_CONST DG_CHAR8* nothing,...) { nothing = nothing; } /* dummy printf to avoid warnings */
#endif

#define SIZE_FOR_NULL_ARRAY (-1)

#define CHECK_SIZE(idx, max, ap) if (idx >= max) {va_end(ap);return DG_ERROR_RPC_TOO_MANY_PAR;}

typedef struct _RPC_Memory
{
    RPC_U32 u32[MAX_SEND_SIZE];
    RPC_U32 u32a[MAX_ARRAY_LENGTH];
    RPC_U16 u16[MAX_SEND_SIZE];
    RPC_U8 u32_idx;
    RPC_U8 u16_idx;
    RPC_U8 u32a_idx;
} RPC_Memory;

/* ==========================================================================
 *
 *  Private Variables
 *
 * ==========================================================================
 */

#ifdef RPC_ARGS_CHECK_ENABLE
RPC_DECL_LOCAL RPC_U8 check_cmd = 0;
#endif

/*
 * see du_rpc.h
 */
void dg_rpc_set_boundary_callbacks(DuagonBoundaryFunc in, DuagonBoundaryFunc out, DG_U8 card_index) {
    dg_card_config[card_index].rpc.in_func = in;
    dg_card_config[card_index].rpc.out_func = out;
}

/*
 * see du_ip.h
 */
RPC_DECL_PUBLIC
void
dg_rpc_send_func
(
    DuagonSendFunc func,
    DG_U8 card_index
)
{
    dg_card_config[card_index].rpc.send_func = func;
}

RPC_DECL_LOCAL
void
memory_init(RPC_Memory *mem)
{
    mem->u32_idx = 0;
    mem->u16_idx = 0;
    mem->u32a_idx = 0;
}

RPC_DECL_LOCAL
RPC_U32 *
memory_u32arr(RPC_Memory *mem, RPC_U32* val, RPC_S32 len)
{
    RPC_S32 i;
    RPC_U32 *retVal;
    retVal = &(mem->u32a[mem->u32a_idx]);
    mem->u32a_idx = (RPC_U8)(mem->u32a_idx + (RPC_U8)len);
    for (i = 0; i < len; i++){
        retVal[i] = val[i];
    }
    return retVal;
}

RPC_DECL_LOCAL
RPC_U32 *
memory_u32val(RPC_Memory *mem, RPC_U32 val)
{
    RPC_U32 *retVal;
    retVal = &(mem->u32[mem->u32_idx++]);
    *retVal = val;
    return retVal;
}

RPC_DECL_LOCAL
RPC_S32 *
memory_s32val(RPC_Memory *mem, RPC_S32 val)
{
    RPC_S32 *retVal;
    retVal = (RPC_S32 *)&(mem->u32[mem->u32_idx++]);
    *retVal = val;
    return retVal;
}



RPC_DECL_LOCAL
RPC_U16 *
memory_u16val(RPC_Memory *mem, RPC_U16 val)
{
    RPC_U16 *retVal;
    retVal = &(mem->u16[mem->u16_idx++]);
    *retVal = val;
    return retVal;
}

#ifdef RPC_ARGS_CHECK_ENABLE
RPC_DECL_LOCAL
void
dg_rpc_check_argument
(
    RPC_U8 protocol,
    RPC_U8 command,
    RTM_HANDLE_P handle,
    RPC_U8 *args,
    RPC_S32 argsSize
)
{
    RPC_S32   retVal;
    RPC_U8    rpc_do_ret;
    void      *recv_data[1];                    /* Array with pointers to data blocks */
    RPC_U32   recv_data_size[1];                /* Array with size (bytes) of the data blocks */
    void      *send_data[4];                   /* Array with pointers to data blocks */
    RPC_U32   send_data_size[4];                /* Array with size (bytes) of the data blocks */
    RPC_PROTOCOL_DATA protocol_send;
    RPC_PROTOCOL_DATA protocol_recv;
    send_data[0] = &check_cmd;
    send_data_size[0] = sizeof(RPC_U8);
    send_data[1] = &command;
    send_data_size[1] = sizeof(RPC_U8);
    send_data[3] = args;
    send_data_size[3] = argsSize; /* first set arg 3 because of SWAP */
    RPC_MACRO_SWAP32(&argsSize);
    send_data[2] = &argsSize;
    send_data_size[2] = sizeof(RPC_S32);
    recv_data[0] = &retVal;
    recv_data_size[0] = sizeof(RPC_S32);

    protocol_send.p_data = send_data;
    protocol_send.nb_bytes = send_data_size;
    protocol_send.nb_entry = 4;

    protocol_recv.p_data = recv_data;
    protocol_recv.nb_bytes = recv_data_size;
    protocol_recv.nb_entry = 1;

    rpc_do_ret = send_func(protocol, handle, &protocol_send, &protocol_recv);
    if (rpc_do_ret != 0)
    {
        rpc_debug_printf("not matching arguments ... exiting(%d) retVal = %i\n", rpc_do_ret, retVal);
        exit(1);
    }
    RPC_MACRO_SWAP16(&retVal);
    if (retVal >= 0)
    {
        rpc_debug_printf("not matching arguments (after swap)... exiting(%d)\n", rpc_do_ret);
        exit(2);
    }
}
#endif


DG_RESULT
dg_rpc_private
(
    RPC_U8 protocol,
    RPC_U8 command,
    RTM_HANDLE_P handle,
    va_list ap
)
{
    RPC_Memory mem;
    void      *recv_data[MAX_RECV_SIZE];                    /* Array with pointers to data blocks */
    RPC_U32   recv_data_size[MAX_RECV_SIZE];                /* Array with size (bytes) of the data blocks */
    void       *send_data[MAX_SEND_SIZE];                   /* Array with pointers to data blocks */
    RPC_U32   send_data_size[MAX_SEND_SIZE];                /* Array with size (bytes) of the data blocks */
    RPC_U8    arg_array[MAX_SEND_SIZE + MAX_RECV_SIZE];
    RPC_U8    arg_idx = 0;
    RPC_U8    arg_size = 0;
    RPC_PROTOCOL_DATA protocol_send;
    RPC_PROTOCOL_DATA protocol_recv;
    RPC_S32   rpc_do_ret;
    RPC_U32 send_idx = 0;
    RPC_U32 recv_idx = 0;
    RPC_U8 *text;
    RPC_U32* u32a;
    RPC_U8 *u8p;
    RPC_U32 *u32p;
    RPC_U16 *u16p;
    RPC_S32 *s32p;
    RPC_S32 tmp_size;
    int arg;
    int i;
    DG_U8 card_index = get_card_index(handle);

    if (dg_card_config[card_index].rpc.in_func!=NULL) {
        dg_card_config[card_index].rpc.in_func(protocol, command, card_index);
    }
    memory_init(&mem);
    send_data[send_idx] = &command;
    send_data_size[send_idx] = sizeof(RPC_U8);
    send_idx++;

    while ((arg = va_arg(ap, int)) != RPC_END)
    {
        arg_array[arg_size++] = (RPC_U8)arg;
        switch (arg)
        {
            case RPC_ARRAY8:
                CHECK_SIZE(send_idx+1, MAX_SEND_SIZE, ap);
                text = (RPC_U8*)va_arg(ap, RPC_CHAR8*);
                send_data[send_idx+1] = (void*)text;
                s32p = va_arg(ap, RPC_S32*);
                if (text == NULL)
                {
                    send_data_size[send_idx+1] = 0;
                    tmp_size = SIZE_FOR_NULL_ARRAY;
                    s32p = &tmp_size;
                }
                else
                {
                    s32p = memory_s32val(&mem, *s32p);
                    send_data_size[send_idx+1] = *s32p;
                    RPC_MACRO_SWAP32(s32p);
                    send_data[send_idx] = s32p;
                }
                send_data_size[send_idx] = sizeof(RPC_S32);
                send_idx = (RPC_U8)(send_idx + 2);
                break;
            case RPC_ARRAY32:
                CHECK_SIZE(send_idx+1, MAX_SEND_SIZE, ap);
                u32a = (RPC_U32*)va_arg(ap, RPC_U32*); /* array of u32 */
                s32p = va_arg(ap, RPC_S32*); /* pointer to the length of the array */
                send_data_size[send_idx] = sizeof(RPC_S32);
                if (NULL == u32a){
                    send_data_size[send_idx+1] = 0;
                    tmp_size = SIZE_FOR_NULL_ARRAY;
                    s32p = &tmp_size;
                } else {
                    u32a = memory_u32arr(&mem, u32a, *s32p);
                    for (i = 0; i< *s32p; i++) {
                        RPC_MACRO_SWAP32(&u32a[i]);
                    }
                    send_data[send_idx+1] = (void*)u32a;
                    send_data_size[send_idx+1] = sizeof(RPC_U32) * *s32p;
                }
                RPC_MACRO_SWAP32(s32p);
                send_data[send_idx] = s32p;
                send_data_size[send_idx] = sizeof(RPC_U32);
                send_idx += 2;
                break;
            case RPC_8:
                CHECK_SIZE(send_idx, MAX_SEND_SIZE, ap);
                send_data[send_idx] = va_arg(ap, RPC_U8*);
                send_data_size[send_idx] = sizeof(RPC_U8);
                send_idx++;
                break;
            case RPC_16:
                CHECK_SIZE(send_idx, MAX_SEND_SIZE, ap);
                u16p = va_arg(ap, RPC_U16*);
                u16p = memory_u16val(&mem, *u16p);
                RPC_MACRO_SWAP16(u16p);
                send_data[send_idx] = u16p;
                send_data_size[send_idx] = sizeof(RPC_S16);
                send_idx++;
                break;
            case RPC_32:
                CHECK_SIZE(send_idx, MAX_SEND_SIZE, ap);
                u32p = va_arg(ap, RPC_U32*);
                u32p = memory_u32val(&mem, *u32p);
                RPC_MACRO_SWAP32(u32p);
                send_data_size[send_idx] = sizeof(RPC_S32);
                send_data[send_idx] = u32p;
                send_idx++;
                break;
            case RPC_TEXT:
                CHECK_SIZE(send_idx+1, MAX_SEND_SIZE, ap);
                send_data[send_idx+1] = text = va_arg(ap, RPC_U8*);
                if (text == NULL)
                {
                    send_data_size[send_idx+1] = 0;
                    tmp_size = SIZE_FOR_NULL_ARRAY;
                    s32p = &tmp_size;
                }
                else
                {
                    s32p = memory_s32val(&mem, strlen((RPC_CHAR8*)text)+1);
                    send_data_size[send_idx+1] = *s32p;
                }
                RPC_MACRO_SWAP32(s32p);
                send_data[send_idx] = s32p;
                send_data_size[send_idx] = sizeof(RPC_S32);
                send_idx = (RPC_U8)(send_idx + 2);
                break;
            case RPC_RETURN_8:
                CHECK_SIZE(recv_idx, MAX_RECV_SIZE, ap);
                recv_data[recv_idx] = va_arg(ap, RPC_S8*);
                recv_data_size[recv_idx] = sizeof(RPC_S8);
                recv_idx++;
                break;
            case RPC_RETURN_16:
                CHECK_SIZE(recv_idx, MAX_RECV_SIZE, ap);
                recv_data[recv_idx] = va_arg(ap, RPC_S16*);
                recv_data_size[recv_idx] = sizeof(RPC_S16);
                recv_idx++;
                break;
            case RPC_RETURN_32:
                CHECK_SIZE(recv_idx, MAX_RECV_SIZE, ap);
                recv_data[recv_idx] = va_arg(ap, RPC_S32*);
                recv_data_size[recv_idx] = sizeof(RPC_S32);
                recv_idx++;
                break;
            case RPC_IO_32:
                CHECK_SIZE(send_idx, MAX_SEND_SIZE, ap);
                CHECK_SIZE(recv_idx, MAX_RECV_SIZE, ap);
                u32p = va_arg(ap, RPC_U32*);
                recv_data[recv_idx] = u32p;
                recv_data_size[recv_idx] = sizeof(RPC_S32);
                recv_idx++;
                u32p = memory_u32val(&mem, *u32p);
                RPC_MACRO_SWAP32(u32p);
                send_data[send_idx] = u32p;
                send_data_size[send_idx] = sizeof(RPC_S32);
                send_idx++;
                break;
            case RPC_IO_16:
                CHECK_SIZE(send_idx, MAX_SEND_SIZE, ap);
                CHECK_SIZE(recv_idx, MAX_RECV_SIZE, ap);
                u16p = va_arg(ap, RPC_U16*);
                recv_data[recv_idx] = u16p;
                recv_data_size[recv_idx] = sizeof(RPC_S16);
                recv_idx++;
                u16p = memory_u16val(&mem, *u16p);
                RPC_MACRO_SWAP16(u16p);
                send_data[send_idx] = u16p;
                send_data_size[send_idx] = sizeof(RPC_S16);
                send_idx++;
                break;
            case RPC_IO_8:
                CHECK_SIZE(send_idx, MAX_SEND_SIZE, ap);
                CHECK_SIZE(recv_idx, MAX_RECV_SIZE, ap);
                u8p = va_arg(ap, RPC_U8*);
                recv_data[recv_idx] = u8p;
                recv_data_size[recv_idx] = sizeof(RPC_U8);
                recv_idx++;
                send_data[send_idx] = u8p;
                send_data_size[send_idx] = sizeof(RPC_S16);
                send_idx++;
                break;
            case RPC_RETURN_ARRAY8:
                CHECK_SIZE(send_idx, MAX_SEND_SIZE, ap);
                CHECK_SIZE(recv_idx+1, MAX_RECV_SIZE, ap);
                text = (RPC_U8*)va_arg(ap, RPC_CHAR8*);
                recv_data[recv_idx+1] = (void*)text;
                s32p = va_arg(ap, RPC_S32*);
                if (text == NULL)
                {   /* need a var because s32p maybe NULL */
                    recv_data[recv_idx] = memory_s32val(&mem, 0);
                    recv_data_size[recv_idx+1] = 0;
                    tmp_size = SIZE_FOR_NULL_ARRAY;
                    s32p = &tmp_size;
                }
                else
                {
                    recv_data[recv_idx] = s32p;  /* need pointer here */
                    s32p = memory_s32val(&mem, *s32p);
                    recv_data_size[recv_idx+1] = *s32p;
                }
                recv_data_size[recv_idx] = sizeof(RPC_U32);
                recv_idx = (RPC_U8)(recv_idx + 2);
                RPC_MACRO_SWAP32(s32p);
                send_data[send_idx] = s32p;
                send_data_size[send_idx] = sizeof(RPC_U32);
                send_idx++;
                break;
            default:
                rpc_debug_printf("dg_rpc_private: unknown RPC type\n");
                return DG_ERROR_RPC_UNKNOWN_TYPE;
        }
    }

#ifdef RPC_ARGS_CHECK_ENABLE
    dg_rpc_check_argument(protocol, command, handle, arg_array, arg_size); /* add one argument to check the 'end-marker'*/
#endif

    protocol_send.p_data = send_data;
    protocol_send.nb_bytes = send_data_size;
    protocol_send.nb_entry = send_idx;

    protocol_recv.p_data = recv_data;
    protocol_recv.nb_bytes = recv_data_size;
    protocol_recv.nb_entry = recv_idx;

    rpc_do_ret = dg_card_config[card_index].rpc.send_func(protocol, handle, &protocol_send, &protocol_recv);
    if (rpc_do_ret != DG_OK)
    {
        if (handle) rtm_setFinished(handle);
        rpc_debug_printf("dg_rpc_private: send_func() returned %d\n", rpc_do_ret);
        return rpc_do_ret;
    }

    /* don't swap receive buffer if handle is not finished (no data was received) */
    if((handle == NULL) ||                          /* no real time handle */
       ((handle != NULL) && rtm_isFinished(handle)) /* has real time handle */
    )
    {
        /* loop through receive buffer to apply swapping*/
        recv_idx = 0;

        for (arg_idx = 0;arg_idx < arg_size;arg_idx++)
        {
            arg = arg_array[arg_idx];
            switch(arg)
            {
                case RPC_RETURN_8:
                    recv_idx++;
                    break;
                case RPC_RETURN_16:
                    RPC_MACRO_SWAP16(recv_data[recv_idx]);
                    recv_idx++;
                    break;
                case RPC_RETURN_32:
                case RPC_IO_32:
                    RPC_MACRO_SWAP32(recv_data[recv_idx]);
                    recv_idx++;
                    break;
                case RPC_RETURN_ARRAY8:
                    RPC_MACRO_SWAP32(recv_data[recv_idx]);
                    recv_idx= (RPC_U8)(recv_idx + 2);
                    break;
                default:
                    break;
            }
        }
    }

    if (dg_card_config[card_index].rpc.out_func!=NULL) {
        dg_card_config[card_index].rpc.out_func(protocol, command, card_index);
    }
    return DG_OK;
}


DLL
RPC_DECL_PUBLIC
DG_RESULT
dg_rpc
(
    RPC_U8 protocol,
    RPC_U8 command,
    RTM_HANDLE_P handle,
    ...
)
{
    va_list ap;
    RPC_S32 ret;

    va_start(ap, handle);
    ret = dg_rpc_private(protocol, command, handle, ap);
    va_end(ap);
    
    return ret;
}
