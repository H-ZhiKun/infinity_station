/* ==========================================================================
 *
 *  File      : DG_SVC.H
 *
 *  Purpose   : Service Protocol API
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

#ifndef DG_SRV_H
#define DG_SRV_H



/* ==========================================================================
 *
 *  Documentation
 *
 * ==========================================================================

Service protocol
================
The Duagon Service Protocol API consists of two generic functions:
-svc_get() to read information
-svc_set() to write information

The information itself is stored as a tree on the device:

"/" ---+--- "device" -----+--- "product"  |  "D113E-B"
       |                  |               |
       |                  +--- "OP"       |  "d-002150-005865"
	   |                  |               |
	   |                  +--- ...        |
       |                                  |
	   +--- "protocol" ---+--- "posix"    |  "version 1.3"
	   |                  |               |
       |                  +--- "iptcom"   |  "version 2.5"
	   |                  |               |
	   |                  +--- ...        |  ...
       |                                  |
	   |                                  |
	   +--- "if" --- ...                  |
	   |                                  |
	   +--- ...                           |

Every node of the tree contains either
-a list with the names of the sub nodes
-an information string if it is an end node

Structure of the Information Tree
---------------------------------
The example keys showed hereafter is only an example, how the tree could look
like. The exact structure highly depends on the specific product, the
hardware/software version and the current status of the board. Therefore
always use the query structure (see function svc_get) in order to build the
information tree.

Example Keys General Information (device):

Key                    | Node Content                |R/W| Example value
-----------------------+-----------------------------+---+-------------------
device_product         | Name of the product or      | R | "D113E-B"
                       | product family              |   |
                       |                             |   |
device_op              | OP number                   | R | "d-001235-004568"
                       |                             |   |
device_app             | Application Identifier      | R | "d-001235-004568"
                       |                             |   |
device_pld             | PLD Version                 | R | "d-001235-004568"
                       |                             |   |
device_cnt_watchdog(1) | Watchdog reset counter      | R | "0"
                       |                             |   |
device_cnt_reset(1)    | Reset counter               | R | "3"
                       |                             |   |
device_startup         | Result of startup sequence  | R | "Ok" / "Error"
                       |                             |   |
device_time            | Local time on the device in |R/W| "500"
                       | seconds since system start  |   |
                       |                             |   |
device_systemLED2)     | Access the system LED       |R/W| "15"

Remarks:
1)The watchdog reset counter is only incremented by a watchdog triggered
reset, whereas the reset counter is incremented through all possible reset
sources (watchdog, host device etc.). Cutting the power supply of the device
will set both counters to zero.
2) The values read from and written to this service key form a bit register,
where each LED can be switched on (1) or off (0). The LSB corresponds to the
first LED. The value can be set as decimal or as hexadecimal formatted
value. Examples:

0xFF 8 LEDs; all LEDs on
0x00 all LEDs off
0x04 third LED on


Example Keys Protocol Related Information (protocol):

Key                    | Node Content                |R/W| Example value
-----------------------+-----------------------------+---+-------------------
protocol_service       | Version of the service      | R | "version 1.0"
                       | protocol                    |   |
                       |                             |   |
protocol_sockets       | Version of the TCP/UDP      | R | "version 1.1"
                       | Socket protocol             |   |
                       |                             |   |
protocol_posix         | Version of the POSIX file   | R | "version 1.0"
                       | protocol                    |   |
                       |                             |   |
protocol_pfs           | Version of the PFS          | R | "version 1.0"
                       |                             |   |
                       |                             |   |
protocol_iptcom_pd     | Version of the IPTCom PD    | R | "3040200"
                       | protocol                    |   |
                       |                             |   |
protocol_iptcom_md     | Version of the IPTCom MD    | R | "3040200"
                       | protocol                    |   |


Example Keys Fieldbus Interface Related Information (if):

Key                    | Node Content                |R/W| Example value
-----------------------+-----------------------------+---+-------------------
if_dhcp_hostname (1)   | DHCP hostname               |R/W| "D113E"
                       |                             |   |
if_ntp_ip              | IP address of the NTP       | R | "192.168.0.100"
                       | server                      |   |
                       |                             |   |
if_eth0_ip (2)         | IP address on ETH0          |R/W| "192.168.0.10"
                       |                             |   |
if_eth0_mac            | MAC address on ETH0         | R |"01:02:03:04:05:06"
                       |                             |   |
if_eth0_sm (2)         | Subnet Mask on ETH0         |R/W| "255.255.255.0"
                       |                             |   |
if_eth0_gateway        | Default gateway on ETH0     | R | "192.168.0.1"
                       |                             |   |
if_eth0_up             | Link status of ETH0         | R | "up" or "down"
                       |                             |   |
if_eth0_speed          | Speed of ETH0               | R | "100MBit"/"10MBit"
                       |                             |   |
if_eth0_duplex         | Duplexity of ETH0           | R | "FD"/"HD"
                       |                             |   |
if_eth0_overflow       | Buffer overflow occurred on | R | "0"
                       | ETH0                        |   |
		               |                             |   |
if_eth0_flowcontrol    | Flowcontrol on ETH0 on or   | R | "0"
                       | off                         |   |
                       |                             |   |
if_eth1...             |  <same as for ETH0>         |   |

Remarks:
1)Setting a new DHCP hostname is processed immediately. However, a board
reset or (at least) an interface restart needs do be done, in order to read
new IP address etc. from the DHCP server. After a DHCP request (e.g.
performed on board reset) this service key will return the DHCP Hostname
retrieved from the DHCP Server.

2)Setting the IP address or the subnet mask manually will only write the new
values into device.cfg. A board reset or (at least) an interface restart
needs do be done in order to take over the new values.
After a DHCP request (e.g. performed on board reset) this service keys will
return the values retrieved from the DHCP Server.


Real time handle
================

For the functions of this API, there exist two ways to call the function:

1) Run to completion: The function returns after the result is completely
evaluated. This can take some time, because first the parameters are
transferred to the device, then the function is executed on the device and
finally the result is transferred back to the host.
-> To run functions to completion, set rtHandle NULL
2) Split transaction: The function returns immediately after the parameters
are transferred to the device. The first call to the function provides a
handle (real time handle). The function must be polled with this handle to
detect if the result of the function is available. The host can do some other
work between polling the function.
-> To run functions with real time handle:
   -initial a RTM_HANDLE variable with rtm_init_handle()
   -repeat calling the function with this handle, until rtm_isFinished()
    returns TRUE
   -evaluete the return parameters

   Example for API XYZ:

   #include <dg_rtm.h>
   RTM_HANDLE rtHandle;
   XYZ_S32 xyz_errno;
   rtm_init_handle(&rtHandle);
   while (1)
   {
       xyz_function(...,...,&xyz_errno,&rtHandle);
	   //TODO: check xyz_errno
       if ( rtm_isFinished(&rtHandle) )
       {
           //TODO: process result
           rtm_init_handle(&rtHandle);
       }
       else
       {
           //TODO: do other activities
       }
   }

Polling Mode
------------
The host interface channels support a polling mode. The polling mode is
useful, if multiple threads access the same host interface channel and
polling is not implemented by the application. If the polling mode is
enabled, a thread can call a non real time handle function, without blocking
other threads from accessing the channel. The channel access functions are
internally implemented with real time handle.

The polling mode can be enabled per channel (e.g. in dg_conf.h):
#define DG_CHANNEL_0_POLLING 1
#define DG_CHANNEL_1_POLLING 1

---------------------------------------------------------------------------*/


/* ==========================================================================
 *
 *  Include files
 *
 * ==========================================================================
 */
#include "dg_svc_p.h"


/* ==========================================================================
 *
 *  Definitions
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Error codes
 * --------------------------------------------------------------------------
 */
#define SVC_OK      0
#define SVC_FAILED -1

#define SVC_ESIZE   1
#define SVC_EDEVICE 2

/* --------------------------------------------------------------------------
 *  errno codes
 * --------------------------------------------------------------------------
 */
/*
For host driver and communication related errno codes refert to "dg_error.h"
*/


/* ==========================================================================
 *
 *  Public procedure interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 * Procedure :  svc_init
 *
 * Purpose   :  Initialize the service protocol
 *
 * Parameters:  *svc_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  SVC_OK if successful,  SVC_FAILED in case of a error
 *              (scv_errno set)
 *
 * Remark    :  has to be called before any other servicer protocol function
 * --------------------------------------------------------------------------
 */
SVC_DECL_PUBLIC
SVC_S32
svc_init
(
	SVC_S32      *svc_errno,
	RTM_HANDLE_P rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  svc_get
 *
 * Purpose   :  Used to get an information string (value) for a given tree
 *              leaf (key). Also possible to get information about the tree
 *              structure when appending "_?" to a key prefix
 *
 * Parameters:  *key        -Pointer to a buffer containing a zero terminated
 *                           string representing the key or a prefix with
 *                           "_?" added to query the structure
 *              *value      -Pointer to a buffer where a zero terminated
 *                           string representing the result value is stored
 *              valueLen    -Size of the buffer for returning the result
 *                           value
 *              *svc_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  Returns SVC_OK if the setting was successful or SVC_FAILED
 *              otherwise (scv_errno set)
 *
 * Remarks   :  Examples for tree structure queries:
 *              key = “_?”        Returns the number of tree elements below 
 *                                the root of the tree.
 *              key = “_?x”       Returns the name of the x-th tree entry
 *                                below the root of the tree if existent.
 *                                (x=0: 1st element, x=1: 2nd element etc.)
 *              key = “device_?”  Returns the number of tree elements in the
 *                                ''device'' branch.
 *              key = “device_?x” Returns the name of the x-th tree element
 *                                in the ''device'' branch if existent.
 *                                (x=0: 1st element, x=1: 2nd element etc.)
 * --------------------------------------------------------------------------
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
);


/* --------------------------------------------------------------------------
 * Procedure :  svc_set
 *
 * Purpose   :  Used to set an information string (value) for a given tree
 *              leaf (key)
 *
 * Parameters:  *key        -Pointer to a buffer containing a zero terminated
 *                           string representing the key
 *              *value       -Pointer to a buffer containing a zero
 *                           terminated string representing the value to set
 *              *svc_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  Returns SVC_OK if the setting was successful or SVC_FAILED
 *              otherwise (scv_errno set)
 *
 * Remarks   :  Not all keys of a device allow setting a value! Some can only
 *              be read. Accessing a nonexistent key or using a not supported
 *              access function will result in the return value SVC_FAILED
 * --------------------------------------------------------------------------
 */
SVC_DECL_PUBLIC
SVC_S32
svc_set
(
	SVC_CHAR8 SVC_DECL_CONST *key,
	SVC_CHAR8 SVC_DECL_CONST *value,
	SVC_S32                  *svc_errno,
	RTM_HANDLE_P             rtHandle
);

#endif /* DG_SRV_H */
