/* ==========================================================================
 *
 *  Module      :   TCN_MCS_CGF.C
 *
 *  Purpose     :   General Function Interface
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
/*   MVB Client Driver Software - Common Interface Definitions                */
/******************************************************************************/
#include "tcn_def.h"

/******************************************************************************/
/*   Standard Libraries                                                       */
/******************************************************************************/
#if defined (O_PRINT_DEBUG)
   #include <stdio.h>
#endif

/******************************************************************************/
/*   MVB Client Driver Software                                               */
/******************************************************************************/
#include "tcn_mcs_cpil.h"
#include "tcn_mcs_clch.h"
#include "tcn_mcs_ccb.h"
#include "tcn_mcs_cgf.h"

/******************************************************************************/
/*   For multi-card  related definitions                                      */
/******************************************************************************/
#include "dg_hdio.h"

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
/*   Server Errors                                                            */
/******************************************************************************/
#define CGF_SERVER_ERROR_NONE       0
#define CGF_SERVER_ERROR_MISSING    1
#define CGF_SERVER_ERROR_FAILED     2

/******************************************************************************/
/*   Server Areas                                                             */
/******************************************************************************/
/* Boot Loader Target Information Area (e.g. D103, D113) */
#define CGF_FLASH_BL_TARGET_INFO_BASE        0x00000400
#define CGF_FLASH_BL_TARGET_INFO_SIZE        0x00000100
/* Boot Loader Software Version Information Area (e.g. D103, D113) */
#define CGF_FLASH_BL_SW_VERSION_INFO_BASE    0x00005E00
#define CGF_FLASH_BL_SW_VERSION_INFO_SIZE    0x00000100


/******************************************************************************/
/*                                                                            */
/*   Global Variables                                                         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*   Internal Function Prototypes                                             */
/*                                                                            */
/******************************************************************************/
GF_RESULT      cgf_check_server_status (UNSIGNED16 * server_error, DG_U8 card_index);
GF_RESULT      cgf_restart_device (DG_U8 card_index);
GF_RESULT      cgf_init_device (DG_U8 card_index);
GF_RESULT      cgf_get_callback_info (UNSIGNED16 * number_of_callbacks, DG_U8 card_index);
GF_RESULT      cgf_get_callback_next (BOOLEAN1 bUseCallbackCounter, UNSIGNED32 max_package_number, DG_U8 card_index);
GF_RESULT      cgf_get_callback_packages (UNSIGNED32 max_package_number, DG_U8 card_index);


/******************************************************************************/
/*                                                                            */
/*   Internal Functions                                                       */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_check_server_status                                    */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       server_error     CGF_SERVER_ERROR_...                      */
/*                                                                            */
/*   RETURN VALUE: result           GF_OK, GF_ERROR                           */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function checks the server status.                    */
/*                 If the server is not ready, up to 500 charaters will       */
/*                 be received from the hardware channel (start-up text of    */
/*                 boot loader).                                              */
/*                                                                            */
/******************************************************************************/

GF_RESULT cgf_check_server_status (UNSIGNED16 * server_error, DG_U8 card_index)
{
GF_RESULT   result;

    /*********************/
    /* get server status */
    /*********************/
    result = cpil_get_server_status();
    if (result == CPIL_ERROR_SERVER_MISSING)
    {
        *server_error = CGF_SERVER_ERROR_MISSING;
        return(GF_ERROR);
    } /* if (result == CPIL_ERROR_SERVER_MISSING) */

    /********************************************/
    /* flush the hardware communication buffers */
    /********************************************/
    result = cpil_hardware_flush_idx(card_index);
    if (result != CPIL_OK)
    {
        *server_error = CGF_SERVER_ERROR_FAILED;
        return(GF_ERROR);
    } /* if (result != CPIL_OK) */

    /*********************/
    /* get server status */
    /*********************/
    result = cpil_get_server_status();
    if (result == CPIL_ERROR_SERVER_MISSING)
    {
        *server_error = CGF_SERVER_ERROR_MISSING;
        return(GF_ERROR);
    } /* if (result == CPIL_ERROR_SERVER_MISSING) */
    if (result != CPIL_OK)
    {
        *server_error = CGF_SERVER_ERROR_FAILED;
        return(GF_ERROR);
    } /* if (result != CPIL_OK) */

    *server_error = CGF_SERVER_ERROR_NONE;

    return(GF_OK);

} /* end "cgf_check_server_status" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_restart_device                                         */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                                 pass 0 for single card systems             */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result           GF_OK, GF_ERROR                           */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function restarts the device communication.           */
/*                                                                            */
/******************************************************************************/

GF_RESULT cgf_restart_device (DG_U8 card_index)
{
GF_RESULT      result = GF_OK;
UNSIGNED16     server_error;
#if defined (O_D402)
   UNSIGNED8   package_data[5];
#endif

   /**************************/
   /* check status of server */
   /**************************/
   result = cgf_check_server_status (&server_error, card_index);
   if (result != GF_OK)
   {
      switch (server_error)
      {
         case (CGF_SERVER_ERROR_MISSING):
            /* set device status */
            #if !defined (O_OS_SINGLE)
               cpil_mutex_take (CPIL_MUTEX_DEVICE_STATUS);
            #endif
            dg_card_config[card_index].mscf.cgf_device_status |= GF_DEVICE_STATUS_SERVER_MISSING;
            #if !defined (O_OS_SINGLE)
               cpil_mutex_give (CPIL_MUTEX_DEVICE_STATUS);
            #endif
            break;
         case (CGF_SERVER_ERROR_FAILED):
            /* set device status */
            #if !defined (O_OS_SINGLE)
               cpil_mutex_take (CPIL_MUTEX_DEVICE_STATUS);
            #endif
            dg_card_config[card_index].mscf.cgf_device_status |= GF_DEVICE_STATUS_SERVER_FAILED;
            #if !defined (O_OS_SINGLE)
               cpil_mutex_give (CPIL_MUTEX_DEVICE_STATUS);
            #endif
            break;
         default: break;
      } /* end "switch (result)" */

      result = GF_ERROR;
      return (result);
   } /* end "if (result != GF_OK)" */

   /*********************/
   /* set device status */
   /*********************/
   #if !defined (O_OS_SINGLE)
      cpil_mutex_take (CPIL_MUTEX_DEVICE_STATUS);
   #endif
   dg_card_config[card_index].mscf.cgf_device_status = GF_DEVICE_STATUS_OK;
   dg_card_config[card_index].mscf.cgf_device_status_old = GF_DEVICE_STATUS_OK;
   #if !defined (O_OS_SINGLE)
      cpil_mutex_give (CPIL_MUTEX_DEVICE_STATUS);
   #endif

   #if defined (O_D402)
      /****************************************************/
      /* send 'mcs' monitor command over physical channel */
      /****************************************************/
      package_data[0] = 'M';
      package_data[1] = 'C';
      package_data[2] = 'S';
      package_data[3] = 13;
      package_data[4] = 0;
      cpil_hardware_send_idx (&package_data[0], 4, card_index);
      /* NOTE:                                           */
      /* Falls der MVB Monitor aktiv ist, dann gibt      */
      /* es eine Antwort auf diesen gesendeten Befehl.   */
   #endif

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_CONFIG, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_CONFIG, MCS_FUNCTION_NUMBER_GF_RESTART_DEVICE, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_CONFIG, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_CONFIG, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_GF_RESTART_DEVICE, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         ;

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = GF_ERROR;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = GF_ERROR;
      } /* end "if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_GF_RESTART_DEVICE) == CLCH_OK)" */
   }
   else
   {
      result = GF_ERROR;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_CONFIG, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_CONFIG);

   return (result);

} /* end "cgf_restart_device" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_init_device                                            */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result           GF_OK, GF_ERROR                           */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function initialise the device communication.         */
/*                                                                            */
/******************************************************************************/

GF_RESULT cgf_init_device (DG_U8 card_index)
{
GF_RESULT   result = GF_OK;

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_CONFIG, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_CONFIG, MCS_FUNCTION_NUMBER_GF_INIT_DEVICE, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_CONFIG, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_CONFIG, &dg_card_config[card_index].mscf.cgf_client_life_sign_timeout, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_CONFIG, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_GF_INIT_DEVICE, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&result, card_index);

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = GF_ERROR;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = GF_ERROR;
      } /* end "if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_GF_INIT_DEVICE) == CLCH_OK)" */
   }
   else
   {
      result = GF_ERROR;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_CONFIG, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_CONFIG);

   return (result);

} /* end "cgf_init_device" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_get_callback_info                                      */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                                 pass 0 for single card systems             */
/*                                                                            */
/*   OUTPUT:       number_of_callbacks    number of available callbacks       */
/*                                                                            */
/*   RETURN VALUE: result                 GF_OK, GF_ERROR                     */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This functions gets the number of available callbacks.     */
/*                                                                            */
/******************************************************************************/

GF_RESULT cgf_get_callback_info (UNSIGNED16 * number_of_callbacks, DG_U8 card_index)
{
GF_RESULT   result = GF_OK;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = GF_ERROR;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   *number_of_callbacks = 0;

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_INFO, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_GF_GET_CALLBACK_INFO, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&result, card_index);
         clch_receive_word_idx (number_of_callbacks, card_index);

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = GF_ERROR;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = GF_ERROR;
      } /* end "if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_GF_GET_CALLBACK_INFO) == CLCH_OK)" */
   }
   else
   {
      result = GF_ERROR;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */

   if (result == GF_OK)
   {
      if (*number_of_callbacks == 0)
      {
         clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
      } /* if (*number_of_callbacks == 0) */
   }
   else
   {
      clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
   } /* if (result == GF_OK) */

   return (result);

} /* end "cgf_get_callback_info" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_get_callback_next                                      */
/*                                                                            */
/*   INPUT:        bUseCallbackCounter    FALSE (for O_OS_MULTI),             */
/*                                        TRUE  (for O_OS_SINGLE)             */
/*                 max_package_number     0 (for O_OS_MULTI)                  */
/*                                        max. number of packages, which      */
/*                                        should be sended by the server      */
/*                                        (for O_OS_SINGLE)                   */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 GF_OK, GF_ERROR                     */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This functions requests the next callback from the server. */
/*                 NOTE:                                                      */
/*                 For O_OS_SINGLE the max. number of packages must           */
/*                 be specified.                                              */
/*                                                                            */
/******************************************************************************/

GF_RESULT cgf_get_callback_next (BOOLEAN1 bUseCallbackCounter, UNSIGNED32 max_package_number, DG_U8 card_index)
{
GF_RESULT   result = GF_OK;

   /**********************************/
   /* send call over logical channel */
   /**********************************/
#ifdef O_OS_SINGLE
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);
#endif

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_NEXT, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, bUseCallbackCounter, card_index);
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &max_package_number, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) != CLCH_OK)
   {
      clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
      result = GF_ERROR;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) != CLCH_OK)" */

   return (result);

} /* end "cgf_get_callback_next" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_get_callback_packages                                  */
/*                                                                            */
/*   INPUT:        max_package_number     max. number of packages, which      */
/*                                        should be sended by the server      */
/*                                        (only O_OS_SINGLE)                  */
/*   INPUT:        card_index Card to use in multi-card system                */
/*                            pass 0 for single card systems                  */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 GF_OK, GF_ERROR                     */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This functions requests callback packages from the server. */
/*                 NOTE:                                                      */
/*                 Only for O_OS_SINGLE.                                      */
/*                                                                            */
/******************************************************************************/


GF_RESULT cgf_get_callback_packages (UNSIGNED32 max_package_number, DG_U8 card_index)
{
GF_RESULT   result = GF_OK;

#ifdef O_OS_SINGLE
   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = GF_ERROR;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */
#endif

   /**********************************/
   /* send call over logical channel */
   /**********************************/
#ifdef O_OS_SINGLE
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);
#endif

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_PACKAGES, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &max_package_number, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) != CLCH_OK)
   {
      clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
      result = GF_ERROR;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) != CLCH_OK)" */

   return (result);

} /* end "cgf_get_callback_packages" */


/******************************************************************************/
/*                                                                            */
/*   External Functions                                                       */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_supervisor                                             */
/*                                                                            */
/*   INPUT:        card_index Card to use in multi-card system                */
/*                            pass 0 for single card systems                  */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function is the supervisor, which runs                */
/*                 the device status callback function.                       */
/*                                                                            */
/******************************************************************************/

void cgf_supervisor (DG_U8 card_index)
{
UNSIGNED16                 device_status;
GF_DEVICE_STATUS_CALLBACK  device_status_callback;

   #if !defined (O_OS_SINGLE)
      cpil_sem_take (CPIL_SEM_SUPERVISOR);
   #else
      dg_card_config[card_index].mscf.bCgfSupervisor = FALSE;
   #endif

   /* get local device status */
   #if !defined (O_OS_SINGLE)
      cpil_mutex_take (CPIL_MUTEX_DEVICE_STATUS);
   #endif
   device_status = dg_card_config[card_index].mscf.cgf_device_status_supervisor;
   #if !defined (O_OS_SINGLE)
      cpil_mutex_give (CPIL_MUTEX_DEVICE_STATUS);
   #endif

   /* perform "device status callback" */
   device_status_callback = dg_card_config[card_index].mscf.cgf_device_status_callback;
   if (device_status_callback != NULL)
   {
      device_status_callback (device_status);
   } /* end "if (device_status_callback != NULL)" */

} /* end "cgf_supervisor" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_put_device_status                                      */
/*                                                                            */
/*   INPUT:        device_status                                              */
/*                 card_index Card to use in multi-card system                */
/*                            pass 0 for single card systems                  */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function puts an additional device status.            */
/*                 It also checks the new device status for an error and      */
/*                 cleans up the MVB client if necessary.                     */
/*                                                                            */
/******************************************************************************/

void cgf_put_device_status (UNSIGNED16 device_status, DG_U8 card_index)
{
BOOLEAN1 bCheckDeviceStatus = FALSE;
BOOLEAN1 bPerformCallback = FALSE;

   #if !defined (O_OS_SINGLE)
      cpil_mutex_take (CPIL_MUTEX_DEVICE_STATUS);
   #endif

   /* NOTE: device status != GF_DEVICE_STATUS_OK */
   dg_card_config[card_index].mscf.cgf_device_status |= device_status;

   if (dg_card_config[card_index].mscf.cgf_server_status == CGF_SERVER_STATUS_RUN)
   {
      dg_card_config[card_index].mscf.cgf_server_status = CGF_SERVER_STATUS_ERROR;

      dg_card_config[card_index].mscf.bDone_gf_init_device = FALSE;
      dg_card_config[card_index].mscf.bDone_am_init = FALSE;
      dg_card_config[card_index].mscf.bDone_am_announce_device = FALSE;
      dg_card_config[card_index].mscf.bCgfCallbackBusy = FALSE;

      bCheckDeviceStatus = TRUE;
   }
   else if (dg_card_config[card_index].mscf.cgf_server_status == CGF_SERVER_STATUS_STOP)
   {
      if (((dg_card_config[card_index].mscf.cgf_device_status & GF_DEVICE_STATUS_SERVER_MISSING) == GF_DEVICE_STATUS_SERVER_MISSING) ||
          ((dg_card_config[card_index].mscf.cgf_device_status & GF_DEVICE_STATUS_SERVER_FAILED) == GF_DEVICE_STATUS_SERVER_FAILED))
      {
         dg_card_config[card_index].mscf.cgf_server_status = CGF_SERVER_STATUS_ERROR;
         bCheckDeviceStatus = TRUE;
      } /* end "if (...)" */
   } /* end "if (cgf_device_status == CGF_SERVER_STATUS_RUN)" */

   if (bCheckDeviceStatus == TRUE)
   {
      if (dg_card_config[card_index].mscf.cgf_device_status_old != dg_card_config[card_index].mscf.cgf_device_status)
      {
         bPerformCallback = TRUE;
         dg_card_config[card_index].mscf.cgf_device_status_old = dg_card_config[card_index].mscf.cgf_device_status;
         dg_card_config[card_index].mscf.cgf_device_status_supervisor = dg_card_config[card_index].mscf.cgf_device_status;
      } /* end "if (cgf_device_status_old != cgf_device_status)" */
   } /* end "if (bCheckDeviceStatus == TRUE)" */

   #if !defined (O_OS_SINGLE)
      cpil_mutex_give (CPIL_MUTEX_DEVICE_STATUS);
   #endif

   if (bPerformCallback == TRUE)
   {
      #if !defined (O_OS_SINGLE)
         cpil_sem_give (CPIL_SEM_SUPERVISOR);
         cpil_idle_wait ();
      #else
         dg_card_config[card_index].mscf.bCgfSupervisor = TRUE;
      #endif
   } /* end "if (bPerformCallback == TRUE)" */

} /* end "cgf_put_device_status" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cgf_poll_server_callback                                   */
/*                                                                            */
/*   INPUT:        max_package_number     max. number of packages to receive  */
/*                 card_index             Card to use in multi-card system    */
/*                                        pass 0 for single card systems      */
/*                                                                            */
/*   OUTPUT:       bNoCallbacks           TRUE, if no callbacks available     */
/*                 bCallbackBusy          TRUE, if callback transmission from */
/*                                        SERVER to CLIENT is busy            */
/*                                                                            */
/*   RETURN VALUE: result                 GF_OK, GF_ERROR                     */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function polls for server callback functions.         */
/*                                                                            */
/******************************************************************************/

GF_RESULT cgf_poll_server_callback (UNSIGNED32 max_package_number,
        BOOLEAN1 *bNoCallbacks,
        BOOLEAN1 *bCallbackBusy,
        DG_U8 card_index)
{
GF_RESULT   result = GF_OK;
UNSIGNED32  l_max_package_number;
UNSIGNED16  number_of_callbacks;
UNSIGNED32  number_of_packages_old;
UNSIGNED32  number_of_packages_new;

    *bNoCallbacks  = FALSE;
    *bCallbackBusy = dg_card_config[card_index].mscf.bCgfCallbackBusy;

    l_max_package_number = max_package_number;
#if defined (O_OS_SINGLE)
    if (l_max_package_number == 0)
    {
        l_max_package_number = 5;
    } /* if (l_max_package_number == 0) */
#endif

#if defined (O_POLL_PACKAGE_MAX)
    if (max_package_number > O_POLL_PACKAGE_MAX)
    {
        max_package_number = O_POLL_PACKAGE_MAX;
    } /* if (max_package_number > O_POLL_PACKAGE_MAX) */
#endif /* O_POLL_PACKAGE_MAX */

    if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
    {
        *bNoCallbacks = TRUE;
        return(GF_ERROR);
    } /* if (cgf_server_status != CGF_SERVER_STATUS_RUN) */

    if (dg_card_config[card_index].mscf.bDone_gf_init_device == FALSE)
    {
        *bNoCallbacks = TRUE;
        return(GF_ERROR);
    } /* if (bDone_gf_init_device == FALSE) */

#if !defined (O_OS_SINGLE)
    cpil_mutex_take (CPIL_MUTEX_CALLBACK);
#endif

    /************************/
    /* callback from server */
    /************************/
    if (dg_card_config[card_index].mscf.bCgfCallbackBusy == FALSE)
    {
        /* get callback info */
        result = cgf_get_callback_info (&number_of_callbacks, card_index);
        if (result != GF_OK)
        {
            *bNoCallbacks = TRUE;
#if !defined (O_OS_SINGLE)
            cpil_mutex_give (CPIL_MUTEX_CALLBACK);
#endif
            return(result);
        } /* if (result != GF_OK) */

        if (number_of_callbacks == 0)
        {
            *bNoCallbacks = TRUE;
#if !defined (O_OS_SINGLE)
            cpil_mutex_give (CPIL_MUTEX_CALLBACK);
#endif
            return(GF_OK);
        } /* if (number_of_callbacks == 0) */

#if !defined (O_OS_SINGLE)
        cpil_mutex_give (CPIL_MUTEX_HARDWARE_CHANNEL);
#endif

        /* get next callback from server */
        result = cgf_get_callback_next (TRUE, l_max_package_number, card_index);
        if (result != GF_OK)
        {
            *bNoCallbacks = TRUE;
#if !defined (O_OS_SINGLE)
            cpil_mutex_give (CPIL_MUTEX_CALLBACK);
#endif
            return(result);
        } /* if (result != GF_OK) */

        dg_card_config[card_index].mscf.bCgfCallbackBusy = TRUE;
        *bCallbackBusy   = dg_card_config[card_index].mscf.bCgfCallbackBusy;
    }
    else
    {
        /* get callback packages */
        result = cgf_get_callback_packages (l_max_package_number, card_index);
        if (result != GF_OK)
        {
            *bNoCallbacks = TRUE;
#if !defined (O_OS_SINGLE)
            cpil_mutex_give (CPIL_MUTEX_CALLBACK);
#endif
            return(result);
        } /* if (result != GF_OK) */
    } /* if (bCgfCallbackBusy == FALSE) */

    number_of_packages_old = ccb_get_package_counter_idx (card_index);
    for (;;)
    {
        /*****************************/
        /* wait for received package */
        /*****************************/
        if (clch_wait_for_receive_idx(0, card_index) != CLCH_OK)
        {
            *bNoCallbacks = TRUE;
            clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
#if !defined (O_OS_SINGLE)
            cpil_mutex_give (CPIL_MUTEX_CALLBACK);
#endif
            return(GF_ERROR);
        } /* if (clch_wait_for_receive_idx(0, card_index) != CLCH_OK) */

        /* NOTE:                                                 */
        /* Zu diesem Zeitpunkt wurde mit Sicherheit ein Package  */
        /* empfangen und eventuell ein Callback ausgeführt.      */
        /* Falls ein Callback ausgeführt wurde hat               */
        /* "number_of_packages_new" den Wert 0.                  */
        number_of_packages_new = ccb_get_package_counter_idx (card_index);

        if (number_of_packages_new == 0)
        {
            /* Callback processed */
            dg_card_config[card_index].mscf.bCgfCallbackBusy = FALSE;
            *bCallbackBusy   = dg_card_config[card_index].mscf.bCgfCallbackBusy;
#if !defined (O_OS_SINGLE)
            cpil_mutex_give (CPIL_MUTEX_CALLBACK);
#endif
            return(GF_OK);
        } /* if (number_of_packages_new == 0) */

        if ((number_of_packages_new - number_of_packages_old) == l_max_package_number)
        {
#if defined (O_OS_SINGLE)
            if (max_package_number != 0)
            {
                clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
                return(GF_OK);
            } /* if (max_package_number != 0) */
#endif

#if !defined (O_OS_SINGLE)
            cpil_mutex_give (CPIL_MUTEX_HARDWARE_CHANNEL);
#endif
            /* get callback packages */
            result = cgf_get_callback_packages (l_max_package_number, card_index);
            if (result != GF_OK)
            {
                *bNoCallbacks = TRUE;
#if !defined (O_OS_SINGLE)
                cpil_mutex_give (CPIL_MUTEX_CALLBACK);
#endif
                return(result);
            } /* if (result != GF_OK) */
            number_of_packages_old = ccb_get_package_counter_idx (card_index);
        } /* if (...) */

    } /* for (;;) */
    return result;
} /* end "cgf_poll_server_callback" */

#if defined (O_INIT_GLOBAL_DATA)
/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     gf_init_global_data                                        */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                                 pass 0 for single card systems             */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function initialises global data.                     */
/*                                                                            */
/******************************************************************************/
void FunctionDef gf_init_global_data_idx (DG_U8 card_index);
void FunctionDef gf_init_global_data (void)
{
    gf_init_global_data_idx(DG_DEFAULT_IDX);
}
void FunctionDef gf_init_global_data_idx (DG_U8 card_index)
{

   /*******************************/
   /* initialise global variables */
   /*******************************/
   dg_card_config[card_index].mscf.cgf_server_status = CGF_SERVER_STATUS_STOP;

   dg_card_config[card_index].mscf.bDone_gf_open_device = FALSE;
   dg_card_config[card_index].mscf.bDone_gf_init_device = FALSE;
   dg_card_config[card_index].mscf.bDone_am_init = FALSE;
   dg_card_config[card_index].mscf.bDone_am_announce_device = FALSE;

   /******************************/
   /* initialise local variables */
   /******************************/
   dg_card_config[card_index].mscf.cgf_device_status = GF_DEVICE_STATUS_OK;
   dg_card_config[card_index].mscf.cgf_device_status_old = GF_DEVICE_STATUS_OK;
   dg_card_config[card_index].mscf.cgf_device_status_supervisor = GF_DEVICE_STATUS_OK;

   #if defined (O_OS_SINGLE)
      dg_card_config[card_index].mscf.bCgfSupervisor = FALSE;
   #endif

   dg_card_config[card_index].mscf.bCgfCallbackBusy = FALSE;

   /***********************************************************/
   /* initialise local variables of function "gf_open_device" */
   /***********************************************************/
   dg_card_config[card_index].mscf.bStartUpServerDone = FALSE;
   dg_card_config[card_index].mscf.bInitServerDone = FALSE;
   dg_card_config[card_index].mscf.bInitPilDone = FALSE;

} /* end "gf_init_global_data" */
#endif


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     gf_open_device                                             */
/*                                                                            */
/*   INPUT:        p_device_config           pointer to device configuration  */
/*                 client_life_sign_timeout  [ms], 0=disable                  */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*   OUTPUT:       device_status             GF_DEVICE_STATUS_...             */
/*                                                                            */
/*   RETURN VALUE: result                    GF_OK, GF_ERROR                  */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function opens the device communication.              */
/*                                                                            */
/******************************************************************************/
GF_RESULT FunctionDef gf_open_device (void * p_device_config,
        UNSIGNED32 client_life_sign_timeout,
        GF_DEVICE_ERROR * device_error)
{
    return gf_open_device_idx(p_device_config, client_life_sign_timeout, device_error, DG_DEFAULT_IDX);
}
GF_RESULT FunctionDef gf_open_device_idx
                                     (void *             p_device_config,
                                      UNSIGNED32         client_life_sign_timeout,
                                      GF_DEVICE_ERROR *  device_error,
                                      DG_U8 card_index)
{
GF_RESULT            result = GF_OK;
UNSIGNED16           server_error;

#if !defined (O_INIT_GLOBAL_DATA)
    dg_card_config[card_index].mscf.bStartUpServerDone = FALSE;
    dg_card_config[card_index].mscf.bInitServerDone = FALSE;
    dg_card_config[card_index].mscf.bInitPilDone = FALSE;
#endif



   *device_error = GF_DEVICE_ERROR_OK;

   if (dg_card_config[card_index].mscf.bDone_gf_open_device == TRUE)
   {
      result = GF_ERROR;
      return (result);
   } /* end "if (bDone_gf_open_device == TRUE)" */

   /*******************/
   /* start up server */
   /*******************/
   if (dg_card_config[card_index].mscf.bStartUpServerDone == FALSE)
   {
      if (cpil_start_up_server(p_device_config) != CPIL_OK)
      {
         dg_card_config[card_index].mscf.cgf_device_status = (UNSIGNED16) GF_DEVICE_STATUS_SERVER_MISSING;
         *device_error = GF_DEVICE_ERROR_SERVER_MISSING;
         result = GF_ERROR;
         return (result);
      } /* end "if (cpil_start_up_server(p_device_config) != CPIL_OK)" */

      dg_card_config[card_index].mscf.bStartUpServerDone = TRUE;

   } /* end "if (bStartUpServerDone == FALSE)" */

   /*********************/
   /* initialise server */
   /*********************/
   if (dg_card_config[card_index].mscf.bInitServerDone == FALSE)
   {
      if (cpil_init_server_idx(card_index) != CPIL_OK)
      {
         dg_card_config[card_index].mscf.cgf_device_status = (UNSIGNED16) GF_DEVICE_STATUS_SERVER_FAILED;
         *device_error = GF_DEVICE_ERROR_SERVER_FAILED;
         result = GF_ERROR;
         return (result);
      } /* end "if (cpil_init_server_idx(card_index) != CPIL_OK)" */

      dg_card_config[card_index].mscf.bInitServerDone = TRUE;

   } /* end "if (bInitServerDone == FALSE)" */

   /**************************/
   /* check status of server */
   /**************************/
   result = cgf_check_server_status (&server_error, card_index);
   if (result != GF_OK)
   {
      switch (server_error)
      {
         case (CGF_SERVER_ERROR_MISSING):
            dg_card_config[card_index].mscf.cgf_device_status = (UNSIGNED16) GF_DEVICE_STATUS_SERVER_MISSING;
            *device_error = GF_DEVICE_ERROR_SERVER_MISSING;
            break;
         case (CGF_SERVER_ERROR_FAILED):
            dg_card_config[card_index].mscf.cgf_device_status = (UNSIGNED16) GF_DEVICE_STATUS_SERVER_FAILED;
            *device_error = GF_DEVICE_ERROR_SERVER_FAILED;
            break;
         default: break;
      } /* end "switch (result)" */

      result = GF_ERROR;
      return (result);

   } /* end "if (result != GF_OK)" */

   dg_card_config[card_index].mscf.cgf_device_status = GF_DEVICE_STATUS_OK;

   if (dg_card_config[card_index].mscf.bInitPilDone == FALSE)
   {
      /*********************************/
      /* initialise callback functions */
      /*********************************/
      ccb_init_idx (card_index);

      /*******************************/
      /* initialise logical channels */
      /*******************************/
      clch_init_idx (card_index);

      /******************/
      /* initialise PIL */
      /******************/
      result = cpil_init_idx (card_index);
      if (result != CPIL_OK)
      {
         *device_error = (GF_DEVICE_ERROR)(GF_DEVICE_ERROR_PIL + result);
         result = GF_ERROR;
         return (result);
      } /* end "if (result != CPIL_OK)" */

      dg_card_config[card_index].mscf.bInitPilDone = TRUE;

   } /* end "if (bInitPilDone == FALSE)" */

   /************************/
   /* set global variables */
   /************************/
   dg_card_config[card_index].mscf.bDone_gf_open_device = TRUE;
   dg_card_config[card_index].mscf.cgf_client_life_sign_timeout = client_life_sign_timeout;

   return (result);

} /* end "gf_open_device" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     gf_init_device                                             */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                    GF_OK, GF_ERROR                  */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This initialise the device communication.                  */
/*                                                                            */
/******************************************************************************/

GF_RESULT FunctionDef gf_init_device (void)
{
    return gf_init_device_idx(DG_DEFAULT_IDX);
}
GF_RESULT FunctionDef gf_init_device_idx (DG_U8 card_index)
{
GF_RESULT   result = GF_OK;


   #if !defined (O_OS_SINGLE)
      cpil_mutex_take (CPIL_MUTEX_MVB_API_INIT_CALL);
   #endif

   if (dg_card_config[card_index].mscf.bDone_gf_open_device == FALSE)
   {
      #if !defined (O_OS_SINGLE)
         cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
      #endif
      result = GF_ERROR;
      return (result);
   } /* end "if ((...))" */

   /******************/
   /* clear callback */
   /******************/
   ccb_clear_callback_idx (card_index);

   /**************************/
   /* reset global variables */
   /**************************/
   dg_card_config[card_index].mscf.bDone_gf_init_device = FALSE;
   dg_card_config[card_index].mscf.bDone_am_init = FALSE;
   dg_card_config[card_index].mscf.bDone_am_announce_device = FALSE;

#if defined (O_OS_SINGLE)
   dg_card_config[card_index].mscf.bCgfSupervisor = FALSE;
#endif

   /*******************************/
   /* initialise logical channels */
   /*******************************/
   clch_init_idx (card_index);

   dg_card_config[card_index].mscf.cgf_server_status = CGF_SERVER_STATUS_STOP;

   /******************/
   /* restart device */
   /******************/
   result = cgf_restart_device (card_index);
   if (result != GF_OK)
   {
      #if !defined (O_OS_SINGLE)
         cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
      #endif
      return (result);
   } /* end "if (result != GF_OK)" */

   dg_card_config[card_index].mscf.cgf_server_status = CGF_SERVER_STATUS_RUN;

   result = cgf_init_device (card_index);
   if (result != GF_OK)
   {
      #if !defined (O_OS_SINGLE)
         cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
      #endif
      return (result);
   } /* end "if (result != GF_OK)" */

   dg_card_config[card_index].mscf.bDone_gf_init_device = TRUE;

   #if !defined (O_OS_SINGLE)
      cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
   #endif

   return (result);

} /* end "gf_init_device" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     gf_get_device_status                                       */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       device_status    GF_DEVICE_STATUS_...                      */
/*                                                                            */
/*   RETURN VALUE: result           GF_OK, GF_ERROR                           */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function gets the device status.                      */
/*                                                                            */
/******************************************************************************/

GF_RESULT FunctionDef gf_get_device_status (GF_DEVICE_STATUS * device_status)
{
    return gf_get_device_status_idx(device_status, DG_DEFAULT_IDX);
}
GF_RESULT FunctionDef gf_get_device_status_idx (GF_DEVICE_STATUS * device_status, DG_U8 card_index)
{
GF_RESULT   result = GF_OK;


   #if !defined (O_OS_SINGLE)
      cpil_mutex_take (CPIL_MUTEX_DEVICE_STATUS);
   #endif

   if (device_status != NULL)
   {
      *device_status = dg_card_config[card_index].mscf.cgf_device_status;
   } /* end "if (device_status != NULL)" */

   if (dg_card_config[card_index].mscf.cgf_device_status == GF_DEVICE_STATUS_OK)
   {
      result = GF_OK;
   }
   else
   {
      result = GF_ERROR;
   } /* end "if (cgf_device_status == GF_DEVICE_STATUS_OK)" */

   #if !defined (O_OS_SINGLE)
      cpil_mutex_give (CPIL_MUTEX_DEVICE_STATUS);
   #endif

   return (result);

} /* end "gf_get_device_status" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     gf_set_device_status_callback                              */
/*                                                                            */
/*   INPUT:        device_status_callback    callback routine                 */
/*                                                                            */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                    GF_OK, GF_ERROR                  */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This functions set a device status callback routine.       */
/*                                                                            */
/******************************************************************************/

GF_RESULT FunctionDef gf_set_device_status_callback (
        GF_DEVICE_STATUS_CALLBACK device_status_callback)
{
    return gf_set_device_status_callback_idx(device_status_callback, DG_DEFAULT_IDX);
}
GF_RESULT FunctionDef gf_set_device_status_callback_idx (
        GF_DEVICE_STATUS_CALLBACK device_status_callback,
        DG_U8 card_index)
{
    GF_RESULT   result = GF_OK;

   /* set global variable */
    dg_card_config[card_index].mscf.cgf_device_status_callback = device_status_callback;
    return (result);
} /* end "gf_set_device_status_callback" */


#if defined (O_OS_SINGLE)
/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     gf_poll_callback                                           */
/*                                                                            */
/*   INPUT:        max_package_number     max. number of packages to receive  */
/*                                        0 = process callback if available   */
/*                                                                            */
/*                                        pass 0 for single card systems      */
/*                                                                            */
/*   OUTPUT:       bCallbackBusy          TRUE, if callback transmission from */
/*                                        SERVER to CLIENT is busy            */
/*                                        In that case it is not allowed to   */
/*                                        call any "am_..." procedure.        */
/*                                                                            */
/*   RETURN VALUE: result                 GF_OK, GF_ERROR                     */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function polls for callback functions.                */
/*                                                                            */
/******************************************************************************/

GF_RESULT FunctionDef gf_poll_callback (
        UNSIGNED32 max_package_number,
        BOOLEAN1 *bCallbackBusy)
{
    return gf_poll_callback_idx(max_package_number, bCallbackBusy, DG_DEFAULT_IDX);
}

GF_RESULT FunctionDef gf_poll_callback_idx (
        UNSIGNED32 max_package_number,
        BOOLEAN1 *bCallbackBusy,
        DG_U8 card_index)
{
GF_RESULT   result = GF_OK;
BOOLEAN1    bNoCallbacks;

do
   {
      if (dg_card_config[card_index].mscf.bCgfSupervisor == TRUE)
      {
         /***************************************/
         /* supervisor - device status callback */
         /***************************************/
         cgf_supervisor (card_index);

         result = GF_ERROR;
         return (result);
      } /* end "if (...)" */

      if (result != GF_OK)
      {
         result = GF_ERROR;
         return (result);
      } /* end "if (result != GF_OK)" */

      /************************/
      /* callback from server */
      /************************/
      result = cgf_poll_server_callback (max_package_number, &bNoCallbacks, bCallbackBusy, card_index);

   } while (result != GF_OK);

   return (result);

} /* end "gf_poll_callback" */
#endif


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif
