/* ==========================================================================
 *
 *  File      : DG_ERROR.H
 *
 *  Purpose   : Error codes for Duagon host driver
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

#ifndef DG_ERROR_H
#define DG_ERROR_H




/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */
#include <os_def.h>

/* ==========================================================================
 *
 *  Type definitions
 *
 * ==========================================================================
 */
typedef DG_S32 DG_RESULT;

/* ==========================================================================
 *
 *  Error codes
 *
 * ==========================================================================
 */
#define DG_OK                                   0  /* No error */
#define DG_ERROR                              400  /* General host driver
                                                      error */
#define DG_ERROR_TIMEOUT                      401  /* Timeout on read/write
													  acess */
#define DG_ERROR_COMM_FAIL                    402  /* Comm.failure */

#define DG_ERROR_OSL_SYNC_INIT                500  /* Mutex initialisation
													  failed */
#define DG_ERROR_OSL_SYNC_LOCK                501  /* Mutex lock failed */
#define DG_ERROR_OSL_SYNC_UNLOCK              502  /* Mutex unlock failed */
#define DG_ERROR_OSL_SYNC_DESTROY             503  /* Mutex destroy failed */

#define DG_ERROR_OSL_INIT                     520  /* I/O access
													  initialisation
													  failed */
#define DG_ERROR_OSL_READ                     521  /* Read access failed */
#define DG_ERROR_OSL_WRITE                    522  /* Write access failed */

#define DG_ERROR_OSL_NOT_READY                523  /* device registers not ready */

#define DG_ERROR_HDIO_RECV_WOULD_BLOCK        541  /* Receiving task would
													  block */
#define DG_ERROR_HDIO_SEND_WOULD_BLOCK        542  /* Sending task would
													  block */
#define DG_ERROR_HDIO_UNKNOWN_CHANNEL_TYPE    544  /* Unknown channel type */
#define DG_ERROR_HDIO_UNKNOWN_IO_MODE         545  /* I/O mode unknown */
#define DG_ERROR_HDIO_NOT_SUPPORTED           546  /* Not supported for
													  configured I/O mode */

#define DG_ERROR_HDC_PROTOCOL_MISMATCH        560  /* Received wrong protocol
													  ID */
#define DG_ERROR_HDC_PROTOCOL                 561  /* Error in HDC protocol */
#define DG_ERROR_HDC_HANDLE                   562  /* Handle is wrong */
#define DG_ERROR_HDC_LENGTH_MISSMATCH         563  /* Payload size specified
													  in header not
													  available */

#define DG_ERROR_RPC_UNKNOWN_TYPE             580  /* Unknown RPC type */
#define DG_ERROR_RPC_TOO_MANY_PAR             581  /* To many parameters */

#define DG_ERROR_VERSION_DRIVER_OLD           590  /* host needs a newer device */
#define DG_ERROR_VERSION_DEVICE_OLD           591  /* device requires a newer host */
#define DG_ERROR_EMPTY_BLOCK                  592  /* protocol not ready */
#define DG_ERROR_REGISTER_NOT_CORRECT         593  /* normally means the device has been reset */
#define DG_ERROR_FUNCTION_TIMED_OUT           594  /* time to execute function exceeds limit */
#define DG_ERROR_CHANNEL_TIMED_OUT            595  /* previous function executed on this 
                                                      channel exceeded timout limit */
#define DG_ERROR_CARD_NOT_FOUND				  600  /* card not defined in dg_config.h */

                                                      
const char* str_dg_errorName (DG_S32 errorCode);
int str_dg_errorDescription (DG_S32 errorCode, char* err_str);

#endif /* DG_ERROR_H */

