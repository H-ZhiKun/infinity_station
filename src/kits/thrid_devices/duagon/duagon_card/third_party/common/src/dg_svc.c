/* ==========================================================================
 *
 *  File      : DG_SVC.C
 *
 *  Purpose   : Service Protocol API
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
#include "dg_svc.h"
#include "dg_rpc.h"
#include "dg_hdc.h"

/* ==========================================================================
 *
 *  Service protocol system driver version
 *
 * ==========================================================================
 */
#define SVC_DRIVER_VERSION 0x00010001 /* 0xMMMMmmmm MMMM=major=1 mmmm=minor=1 */

/* ==========================================================================
 *
 * Version of Service Protocol on device
 *
 * ==========================================================================
 */
#define SVC_MIN_PROT_VERSION_MAJOR 1 /* Service protocol on the device must have at least this major value */
#define SVC_MIN_PROT_VERSION_MINOR 3 /* Service protocol on the device must have at least this minor value */


/* ==========================================================================
 *
 *  Public Procedure Interface
 *
 * ==========================================================================
 */
SVC_DECL_PUBLIC
SVC_S32
svc_get
(
	SVC_DECL_CONST SVC_CHAR8 *key,
	SVC_CHAR8                *value,
	SVC_S32                  valueLen,
	SVC_S32                  *svc_errno,
	RTM_HANDLE_P             rtHandle
)
{
	SVC_S32 retVal;
	SVC_S32 rpc_return = 0;
    rpc_return = dg_rpc(
		         SVC_PROTOCOL_ID,
		         CMD_SVC_GET,
		         rtHandle,
		         RPC_TEXT, key,
		         RPC_RETURN_ARRAY8, value, &valueLen,
		         RPC_RETURN_32, &retVal,
		         RPC_END);
	if (rpc_return!=0) 
    {
        retVal=-1;
		*svc_errno = rpc_return;
    }
    else if (SVC_FAILED == retVal)
    {
        *svc_errno = SVC_EDEVICE;
    }
	return retVal;
}


SVC_DECL_PUBLIC
SVC_S32
svc_set
(
	SVC_DECL_CONST SVC_CHAR8 *key,
	SVC_DECL_CONST SVC_CHAR8 *value,
	SVC_S32                  *svc_errno,
	RTM_HANDLE_P             rtHandle
)
{
	SVC_S32 retVal;
	SVC_S32 rpc_return = 0;
    rpc_return = dg_rpc(
		         SVC_PROTOCOL_ID,
		         CMD_SVC_SET,
		         rtHandle,
		         RPC_TEXT, key,
		         RPC_TEXT, value,
		         RPC_RETURN_32, &retVal,
		         RPC_END);
	if (rpc_return!=0) 
    {
        retVal=-1;
		*svc_errno = rpc_return;
    }
    else if (SVC_FAILED == retVal)
    {
        *svc_errno = SVC_EDEVICE;
    }
	return retVal;
}


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
)
{
	SVC_S32 retVal;
	SVC_S32 rpc_return = 0;

    if (SVC_DATA_SIZE_MAX < outLen)
    {
        *svc_errno = SVC_ESIZE;
        return -1;
    }

	rpc_return = dg_rpc(
		SVC_PROTOCOL_ID,
		CMD_SVC_LOOPBACK,
		rtHandle,
		RPC_ARRAY8, outData, &outLen,
		RPC_RETURN_ARRAY8, backData, &backLen,
		RPC_RETURN_32, &retVal,
		RPC_END);
	if (rpc_return!=0) 
    {
	    retVal=-1;
	    *svc_errno = rpc_return;
    }
    else if (SVC_FAILED == retVal)
    {
        *svc_errno = SVC_EDEVICE;
    }
	return retVal;
}


SVC_DECL_PUBLIC
SVC_S32
svc_init
(
	SVC_S32      *svc_errno,
	RTM_HANDLE_P rtHandle
) 
{
	SVC_S32 rpc_return = 0;
	SVC_S32 ret_val = 0;
	RPC_S32 version=SVC_DRIVER_VERSION;

	if ( rtHandle!=NULL )
	{
		ret_val = hdc_init(rtHandle->card_index);
	}
	else
	{
		ret_val = hdc_init(0);
	}
	if (ret_val != DG_OK)
	{
		if (rtHandle) rtm_setFinished(rtHandle);
		*svc_errno = ret_val;
		return -1;
	}
    rpc_return = dg_rpc(
		SVC_PROTOCOL_ID,
		CMD_SVC_INIT,
		rtHandle,
		RPC_IO_32, &version,
		RPC_RETURN_32, &ret_val,
		RPC_RETURN_32, svc_errno,
		RPC_END);
	if ( rpc_return != 0 )
    {
		*svc_errno = rpc_return;
		ret_val=-1;
    }
    else if (SVC_OK != ret_val)
    {
        *svc_errno = SVC_EDEVICE;
    }
	else if ( ((version >> 16)& 0x0000FFFF) < SVC_MIN_PROT_VERSION_MAJOR || (version & 0x0000FFFF) < SVC_MIN_PROT_VERSION_MINOR  )
	{
		*svc_errno = DG_ERROR_VERSION_DEVICE_OLD;
		ret_val=-1;
	}
    return ret_val;
}
