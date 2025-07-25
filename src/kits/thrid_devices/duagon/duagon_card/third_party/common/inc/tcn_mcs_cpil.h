#if !defined (TCN_MCS_CPIL_H)
#define TCN_MCS_CPIL_H

/* ==========================================================================
 *
 *  Module      :   TCN_MCS_CPIL.H
 *
 *  Purpose     :   Processor Interface Library
 *                  - access to host interface of MVB interface product
 *                  - access to operating system specific libraries
 *
 *  Project     :   MVB Client-Server Model (MVB Client Driver Software)
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon AG, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * --------------------------------------------------------------------------
 *
 *
 * ==========================================================================
 */


/******************************************************************************/
/*                                                                            */
/*   Include Files                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*   Application Interface                                                    */
/******************************************************************************/
#include "tcn_def.h"


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (begin)                                            */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif


/******************************************************************************/
/*                                                                            */
/*   General Definitions                                                      */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*   Error Codes                                                              */
/******************************************************************************/
#define CPIL_OK                                 0
#define CPIL_ERROR                              1
#define CPIL_ERROR_INIT_CHANNEL                 2
#define CPIL_ERROR_INIT_MUTEX                   3
#define CPIL_ERROR_INIT_SEM                     4
#define CPIL_ERROR_INIT_TASK_SUPERVISOR         5
#define CPIL_ERROR_INIT_TASK_CALLBACK           6

#define CPIL_ERROR_SERVER_MISSING               10
#define CPIL_ERROR_SERVER_FAILED                11

#if !defined (O_OS_SINGLE)
/******************************************************************************/
/*   Mutual Exclusion                                                         */
/******************************************************************************/
#define CPIL_MUTEX_LOGIC_CHANNEL0      ((UNSIGNED8) 0)
#define CPIL_MUTEX_LOGIC_CHANNEL1      ((UNSIGNED8) 1)
#define CPIL_MUTEX_LOGIC_CHANNEL2      ((UNSIGNED8) 2)
#define CPIL_MUTEX_LOGIC_CHANNEL3      ((UNSIGNED8) 3)
#define CPIL_MUTEX_CALLBACK            ((UNSIGNED8) 4)
#define CPIL_MUTEX_HARDWARE_CHANNEL    ((UNSIGNED8) 5)
#define CPIL_MUTEX_MVB_API_INIT_CALL   ((UNSIGNED8) 6)
#define CPIL_MUTEX_DEVICE_STATUS       ((UNSIGNED8) 7)

#define CPIL_MAX_MUTEX                 ((UNSIGNED8) 8)
#endif

#if !defined (O_OS_SINGLE)
/******************************************************************************/
/*   Semaphores                                                               */
/******************************************************************************/
#define CPIL_SEM_SUPERVISOR            ((UNSIGNED8) 0)
#define CPIL_SEM_CALLBACK              ((UNSIGNED8) 1)

#define CPIL_MAX_SEM                   ((UNSIGNED8) 2)
#endif


/******************************************************************************/
/*                                                                            */
/*   Type Definitions                                                         */
/*                                                                            */
/******************************************************************************/
typedef UNSIGNED16   CPIL_RESULT;


/******************************************************************************/
/*                                                                            */
/*   Function Prototypes                                                      */
/*                                                                            */
/******************************************************************************/

void *      cpil_mem_alloc (UNSIGNED32 mem_size);
void        cpil_mem_free (void * p_mem);
CPIL_RESULT cpil_get_server_status (void);
CPIL_RESULT cpil_start_up_server (void * p_device_config);


#ifndef DG_MULTI_CARD

#if !defined (O_OS_SINGLE)
   void     cpil_mutex_take (UNSIGNED16 mutex_number);
   void     cpil_mutex_give (UNSIGNED16 mutex_number);
   void     cpil_sem_take (UNSIGNED16 sem_number);
   void     cpil_sem_give (UNSIGNED16 sem_number);
   void     cpil_idle_wait (void);
#endif

   CPIL_RESULT cpil_init (void);
   CPIL_RESULT cpil_close (void);
   CPIL_RESULT cpil_init_server (void);
   CPIL_RESULT cpil_wait_for_hardware_receive (UNSIGNED8 number);
   CPIL_RESULT cpil_hardware_send (UNSIGNED8 * p_data, UNSIGNED16 data_size);
   void        cpil_hardware_receive (UNSIGNED8 * p_data);
   CPIL_RESULT cpil_hardware_flush (void);

#endif


CPIL_RESULT cpil_init_idx (DG_U8 card_index);
CPIL_RESULT cpil_close_idx (DG_U8 card_index);
CPIL_RESULT cpil_init_server_idx (DG_U8 card_index);
CPIL_RESULT cpil_wait_for_hardware_receive_idx (UNSIGNED8 number, DG_U8 card_index);
CPIL_RESULT cpil_hardware_send_idx (UNSIGNED8 * p_data, UNSIGNED16 data_size, DG_U8 card_index);
void        cpil_hardware_receive_idx (UNSIGNED8 * p_data, DG_U8 card_index);
CPIL_RESULT cpil_hardware_flush_idx (DG_U8 card_index);


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif


#endif /* !TCN_MCS_CPIL_H */
