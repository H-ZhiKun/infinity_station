/* ==========================================================================
 *
 *  Module      :   TCN_AM.C
 *
 *  Purpose     :   Message Data Interface
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
/*   MVB Client Driver Software                                               */
/******************************************************************************/
#include "tcn_mcs_cpil.h"
#include "tcn_mcs_clch.h"
#include "tcn_mcs_cgf.h"

/******************************************************************************/
/*   MVB Client API                                                           */
/******************************************************************************/
#include "tcn_am.h"

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
#ifndef MCS_DIR_TABLE_SIZE
#define MCS_DIR_TABLE_SIZE    256
#endif
#define CMD_MAX_MESSAGE_SIZE  0xFFFF


/******************************************************************************/
/*                                                                            */
/*   Local Variables                                                          */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*   External Functions                                                       */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_init                                                    */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              see TCN documents                      */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_init".                      */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_init (void)
{
    return am_init_idx(DG_DEFAULT_IDX);
}
AM_RESULT FunctionDef am_init_idx (DG_U8 card_index)
{
    AM_RESULT   result;
    UNSIGNED16  temp_result;
    
    UNSIGNED32      gf_device_config;
    GF_DEVICE_ERROR gf_device_error;
    GF_RESULT       gf_result;

    gf_result = gf_open_device_idx(&gf_device_config, 0, &gf_device_error, card_index);
    if( gf_result != GF_OK )
    {
        result = AM_FAILURE;
    }
    else
    {
        /* ----------------------------------------------------------------------
        *  gf_init_device()
        * ----------------------------------------------------------------------
        */
        gf_result = gf_init_device_idx(card_index);
        if( gf_result != GF_OK )
        {
            result = AM_FAILURE;
        }
        else
        {

            if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
            {
            result = AM_FAILURE;
            return (result);
            } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */
            
            #if !defined (O_OS_SINGLE)
            cpil_mutex_take (CPIL_MUTEX_MVB_API_INIT_CALL);
            #endif
            
            if (dg_card_config[card_index].mscf.bDone_am_init == TRUE)
            {
            #if !defined (O_OS_SINGLE)
                cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
            #endif
            result = AM_FAILURE;
            return (result);
            } /* end "if ((...))" */
            
            /**********************************/
            /* send call over logical channel */
            /**********************************/
            clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);
            
            clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_INIT, card_index);
            clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
            
            if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
            {
            /*****************************/
            /* wait for return parameter */
            /*****************************/
            if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_INIT, card_index) == CLCH_OK)
            {
                /************************/
                /* get return parameter */
                /************************/
                clch_receive_word_idx (&temp_result, card_index);
                result = (AM_RESULT)temp_result;
            
#if defined (O_RECEIVE_CRC)
                    /*************/
                    /* check CRC */
                    /*************/
                    if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
                    {
                        result = AM_FAILURE;
                    } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
        
                if (result == AM_OK)
                    {
                        dg_card_config[card_index].mscf.bDone_am_init = TRUE;
                        am_clear_dir_mvb_idx (card_index);
                    } /* end "if (result == AM_OK)" */
                }
                else
                {
                    result = AM_FAILURE;
                } /* end "if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_INIT) == CLCH_OK)" */
            }
            else
            {
                result = AM_FAILURE;
            } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */
            
            clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
            
            #if !defined (O_OS_SINGLE)
                cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
            #endif
        }
    }    
    return (result);

} /* end "am_init" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_announce_device                                         */
/*                                                                            */
/*   INPUT:        max_call_number        see TCN documents                   */
/*                 max_inst_number        see TCN documents                   */
/*                 default_reply_timeout  see TCN documents                   */
/*                 my_credit              see TCN documents                   */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 see TCN documents                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_announce_device".           */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_announce_device
(
   UNSIGNED16  max_call_number,
   UNSIGNED16  max_inst_number,
   UNSIGNED16  default_reply_timeout,
   UNSIGNED8   my_credit
)
{
    return am_announce_device_idx(max_call_number, max_inst_number, default_reply_timeout, my_credit, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_announce_device_idx (UNSIGNED16   max_call_number,
                       UNSIGNED16   max_inst_number,
                       UNSIGNED16   default_reply_timeout,
                       UNSIGNED8    my_credit,
                       DG_U8 card_index)
{
AM_RESULT   result;
UNSIGNED16  temp_result;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   #if !defined (O_OS_SINGLE)
      cpil_mutex_take (CPIL_MUTEX_MVB_API_INIT_CALL);
   #endif

   if ((dg_card_config[card_index].mscf.bDone_am_init == FALSE)  ||
       (dg_card_config[card_index].mscf.bDone_am_announce_device == TRUE))
   {
      #if !defined (O_OS_SINGLE)
         cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
      #endif
      result = AM_FAILURE;
      return (result);
   } /* end "if ((...))" */

   /* limitation of max calls and replier instances */
   if ((max_call_number > 256) || (max_inst_number > 256))
   {
      #if !defined (O_OS_SINGLE)
         cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
      #endif
      result = AM_FAILURE;
      return (result);
   } /* end "if ((max_call_number > 256) || (max_inst_number > 256))" */

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_ANNOUNCE_DEVICE, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
   clch_channel_send_word_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &max_call_number, card_index);
   clch_channel_send_word_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &max_inst_number, card_index);
   clch_channel_send_word_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &default_reply_timeout, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, my_credit, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_ANNOUNCE_DEVICE, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&temp_result, card_index);
         result = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = AM_FAILURE;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif

         if (result == AM_OK)
         {
            dg_card_config[card_index].mscf.bDone_am_announce_device = TRUE;
         } /* end "if (result == AM_OK)" */
      }
      else
      {
         result = AM_FAILURE;
      } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_ANNOUNCE_DEVICE) == CLCH_OK)" */
   }
   else
   {
      result = AM_FAILURE;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);

   #if !defined (O_OS_SINGLE)
      cpil_mutex_give (CPIL_MUTEX_MVB_API_INIT_CALL);
   #endif

   #if !defined (O_OS_SINGLE)
      cpil_sem_give (CPIL_SEM_CALLBACK);
   #endif

   return (result);

} /* end "am_announce_device" */

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_poll_callback                                           */
/*                                                                            */
/*   INPUT:        max_package_number     max. number of packages to receive  */
/*                                        0 = process callback if available   */
/*                                                                            */
/*   OUTPUT:       bCallbackBusy          TRUE, if callback transmission from */
/*                                        SERVER to CLIENT is busy            */
/*                                        In that case it is not allowed to   */
/*                                        call any "am_..." procedure.        */
/*                                                                            */
/*   RETURN VALUE: result                 AM_OK, AM_FAILURE                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function polls for callback functions.                */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_poll_callback (UNSIGNED32 max_package_number, BOOLEAN1 *bCallbackBusy)
{
    return am_poll_callback_idx (max_package_number, bCallbackBusy, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef am_poll_callback_idx (UNSIGNED32 max_package_number, BOOLEAN1 *bCallbackBusy, DG_U8 card_index)
{
    AM_RESULT   result;
    GF_RESULT   gf_result;

    result = AM_OK;

    gf_result = gf_poll_callback_idx(max_package_number, bCallbackBusy, card_index);
    if (gf_result != GF_OK)
    {
        result = AM_FAILURE;
    }

    return (result);

} /* end "am_poll_callback" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_set_current_tc                                          */
/*                                                                            */
/*   INPUT:        this_topo              see TCN documents                   */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 see TCN documents                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_set_current_tc".            */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_set_current_tc (UNSIGNED8 this_topo)

{
    return am_set_current_tc_idx(this_topo ,DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_set_current_tc_idx (UNSIGNED8 this_topo, DG_U8 card_index)
{
AM_RESULT   result;
UNSIGNED16  temp_result;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_announce_device == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_announce_device == FALSE)" */

   if (this_topo >= 63)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (this_topo >= 63)" */

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_SET_CURRENT_TC, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, this_topo, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_SET_CURRENT_TC, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&temp_result, card_index);
         result = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = AM_FAILURE;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = AM_FAILURE;
      } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_SET_CURRENT_TC) == CLCH_OK)" */
   }
   else
   {
      result = AM_FAILURE;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);

   return (result);

} /* end "am_set_current_tc" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_stadi_write                                             */
/*                                                                            */
/*   INPUT:        entries                see TCN documents                   */
/*                 nr_of_entries          see TCN documents                   */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 see TCN documents                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_stadi_write".               */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_stadi_write
(
   const AM_STADI_ENTRY entries[],
   UNSIGNED8            nr_of_entries
)
{
    return am_stadi_write_idx(entries, nr_of_entries, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_stadi_write_idx
   (const AM_STADI_ENTRY   entries[],
    UNSIGNED8              nr_of_entries,
    DG_U8                  card_index)
{
AM_RESULT   result = AM_OK;
UNSIGNED16  counter;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_init == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_init == FALSE)" */

   /********************/
   /* perform function */
   /********************/
   for (counter=0; counter<nr_of_entries; counter++)
   {
      if ((entries[counter].station != entries[counter].next_station) ||
          (entries[counter].station != (UNSIGNED8)(entries[counter].device_adr)) ||
          (entries[counter].bus_id != 0))
      {
         result = AM_FAILURE;
      } /* end "if ((...))" */
   } /* end "for (counter=0; counter<nr_of_entries; counter++)" */

   return (result);

} /* end "am_stadi_write" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_stadi_read                                              */
/*                                                                            */
/*   INPUT:        entries                see TCN documents                   */
/*                 nr_of_entries          see TCN documents                   */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 see TCN documents                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_stadi_read".                */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_stadi_read
(
    AM_STADI_ENTRY entries[],
    UNSIGNED8 nr_of_entries
)
{
    return am_stadi_read_idx(entries, nr_of_entries, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef am_stadi_read_idx
(
    AM_STADI_ENTRY entries[],
    UNSIGNED8 nr_of_entries,
    DG_U8          card_index
)
{
AM_RESULT   result = AM_OK;
UNSIGNED16  counter;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_init == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_init == FALSE)" */

   /********************/
   /* perform function */
   /********************/
   for (counter=0; counter<nr_of_entries; counter++)
   {
      entries[counter].next_station = entries[counter].station;
      entries[counter].device_adr = entries[counter].station;
      entries[counter].bus_id = 0;
   } /* end "for (counter=0; counter<nr_of_entries; counter++)" */

   return (result);

} /* end "am_stadi_read" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_clear_dir                                               */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 see TCN documents                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_clear_dir".                 */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_clear_dir (void)
{
    return am_clear_dir_mvb_idx(DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef am_clear_dir_mvb_idx (DG_U8 card_index)
{
AM_RESULT   result = AM_OK;
UNSIGNED16  counter;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_init == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_init == FALSE)" */

   /********************/
   /* perform function */
   /********************/
   for (counter=0; counter<MCS_DIR_TABLE_SIZE; counter++)
   {
      dg_card_config[card_index].mscd.mcs_dir_table[counter] = AM_UNKNOWN;
   } /* end "for (counter=0; counter<MCS_DIR_TABLE_SIZE; counter++)" */

   return (result);

} /* end "am_clear_dir" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_insert_dir_entries                                      */
/*                                                                            */
/*   INPUT:        function_list          see TCN documents                   */
/*                 number_of_entries      see TCN documents                   */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 see TCN documents                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_insert_dir_entries".        */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_insert_dir_entries
(
   AM_DIR_ENTRY * function_list,
   UNSIGNED8      number_of_entries
)
{
    return am_insert_dir_entries_idx(function_list, number_of_entries, DG_DEFAULT_IDX);
}
AM_RESULT FunctionDef am_insert_dir_entries_idx
(
   AM_DIR_ENTRY * function_list,
   UNSIGNED8      number_of_entries,
   DG_U8 card_index
)
{
AM_RESULT   result = AM_OK;
UNSIGNED16  counter;


   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_init == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_init == FALSE)" */

   /********************/
   /* perform function */
   /********************/
   for (counter=0; counter<number_of_entries; counter++)
   {
      dg_card_config[card_index].mscd.mcs_dir_table[function_list[counter].function] = function_list[counter].station;
   } /* end "for (counter=0; counter<number_of_entries; counter++)" */

   return (result);

} /* end "am_insert_dir_entries" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_remove_dir_entries                                      */
/*                                                                            */
/*   INPUT:        function_list          see TCN documents                   */
/*                 number_of_entries      see TCN documents                   */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result                 see TCN documents                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_remove_dir_entries".        */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_remove_dir_entries
(
   AM_DIR_ENTRY * function_list,
   UNSIGNED8      number_of_entries
)
{
    return am_remove_dir_entries_idx ( function_list, number_of_entries, DG_DEFAULT_IDX);
}
AM_RESULT FunctionDef am_remove_dir_entries_idx
(
   AM_DIR_ENTRY * function_list,
   UNSIGNED8      number_of_entries,
   DG_U8 card_index
)
{
AM_RESULT   result = AM_OK;
UNSIGNED16  counter;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_init == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_init == FALSE)" */

   /********************/
   /* perform function */
   /********************/
   for (counter=0; counter<number_of_entries; counter++)
   {
      dg_card_config[card_index].mscd.mcs_dir_table[function_list[counter].function] = AM_UNKNOWN;
   } /* end "for (counter=0; counter<number_of_entries; counter++)" */

   return (result);

} /* end "am_remove_dir_entries" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_get_dir_entry                                           */
/*                                                                            */
/*   INPUT:        function               see TCN documents                   */
/*                                                                            */
/*   OUTPUT:       station                see TCN documents                   */
/*                                                                            */
/*   RETURN VALUE: result                 see TCN documents                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_get_dir_entry".             */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_get_dir_entry
(
   UNSIGNED8   function,
   UNSIGNED8 * station
)
{
    return am_get_dir_entry_idx (function, station, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef am_get_dir_entry_idx
(
   UNSIGNED8   function,
   UNSIGNED8 * station,
   DG_U8 card_index
)
{
AM_RESULT   result = AM_OK;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_init == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_init == FALSE)" */

   /********************/
   /* perform function */
   /********************/
   *station = dg_card_config[card_index].mscd.mcs_dir_table[function];

   return (result);

} /* end "am_get_dir_entry" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_call_request                                            */
/*                                                                            */
/*   INPUT:        caller_function     see TCN documents                      */
/*                 replier             see TCN documents                      */
/*                 out_msg_adr         see TCN documents                      */
/*                 out_msg_size        see TCN documents                      */
/*                 in_msg_adr          see TCN documents                      */
/*                 in_msg_size         see TCN documents                      */
/*                 reply_timeout       see TCN documents                      */
/*                 call_confirm        see TCN documents                      */
/*                 caller_ref          see TCN documents                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_call_request".              */
/*                                                                            */
/******************************************************************************/

void FunctionDef am_call_request
(
   UNSIGNED8 caller_function,
   const AM_ADDRESS * replier,
   void * out_msg_adr, UNSIGNED32 out_msg_size,
   void * in_msg_adr, UNSIGNED32 in_msg_size,
   UNSIGNED16 reply_timeout,
   AM_CALL_CONFIRM call_confirm,
   void * caller_ref
)
{
    am_call_request_idx(caller_function, replier, out_msg_adr,
            out_msg_size, in_msg_adr, in_msg_size, reply_timeout, call_confirm, caller_ref, 0);
}
void FunctionDef am_call_request_idx
(
   UNSIGNED8 caller_function,
   const AM_ADDRESS * replier,
   void * out_msg_adr, UNSIGNED32 out_msg_size,
   void * in_msg_adr, UNSIGNED32 in_msg_size,
   UNSIGNED16 reply_timeout,
   AM_CALL_CONFIRM call_confirm,
   void * caller_ref,
   DG_U8 card_index
)
{
AM_RESULT   status = AM_OK;
UNSIGNED16  temp_result;
AM_ADDRESS  local_replier;
UNSIGNED8   func_or_stat = AM_ROUTER_FCT;
UNSIGNED8   next_station;
BOOLEAN1    bGetDirEntry = TRUE;
UNSIGNED8   in_msg_dynamic;
BOOLEAN1    bCallConfirm = FALSE;
BOOLEAN1    bFirstPackage;


   /* avoid warnings */
   local_replier.sg_node      = 0;
   local_replier.func_or_stat = 0;
   local_replier.next_station = 0;
   local_replier.topo_counter = 0;

   if (call_confirm == NULL)
   {
      return;
   } /* end "if (call_confirm == NULL)" */

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      status = AM_FAILURE;
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (status == AM_OK)
   {
      if (dg_card_config[card_index].mscf.bDone_am_announce_device == FALSE)
      {
         status = AM_FAILURE;
      } /* end "if (bDone_am_announce_device == FALSE)" */
   } /* end "if (status == AM_OK)" */

   if (status == AM_OK)
   {
      /*********************/
      /* set local replier */
      /*********************/
      local_replier = *replier;

      /* NOTE:                               */
      /* See                                 */
      /* - IEC 61375-2/FDIS,                 */
      /* - Train Communication Network,      */
      /* - Clause 2: Real-Time Protocols,    */
      /* - 2.3.10.3.3 System or User (snu)   */
      if ((local_replier.sg_node & AM_SYSTEM_ADDR) == AM_SYSTEM_ADDR)
      {
         /* system address */
         if (caller_function != AM_MANAGER_FCT)
         {
            status = AM_ADDR_FMT_ERR;
         } /* end "if (caller_function != AM_MANAGER_FCT)" */
      } /* end "if ((local_replier.sg_node & AM_SYSTEM_ADDR) == AM_SYSTEM_ADDR)" */
   } /* end "if (status == AM_OK)" */

   if (status == AM_OK)
   {
      /****************************************************/
      /* check, if next_station = AM_UNKNOWN (AM_ADDRESS) */
      /****************************************************/
      next_station = local_replier.next_station;
      if (next_station == AM_UNKNOWN)
      {
         if ((local_replier.sg_node & ~AM_SYSTEM_ADDR) != AM_SAME_NODE)
         {
            /* send over train bus */
            func_or_stat = AM_ROUTER_FCT;
         }
         else
         {
            /* don't send over train bus */
            if ((local_replier.sg_node & AM_SYSTEM_ADDR) == AM_SYSTEM_ADDR)
            {
               /* system address */
               bGetDirEntry = FALSE;

               if (local_replier.func_or_stat == AM_UNKNOWN)
               {
                  /* NOTE:                                           */
                  /* See                                             */
                  /* - IEC 61375-2/FDIS,                             */
                  /* - Train Communication Network,                  */
                  /* - Clause 2: Real-Time Protocols,                */
                  /* - 2.3.10.3.6 Station or function (func_or_stat) */
                  status = AM_ADDR_FMT_ERR;
                  /* NOTE:                            */
                  /* In TCN software from ADtranz set */
                  /* next_station=own_device_address. */
               } /* end "if (local_replier.func_or_stat == AM_UNKNOWN)" */
            }
            else
            {
               /* user address */
               func_or_stat = local_replier.func_or_stat;
            } /* end "if ((local_replier.sg_node & AM_SYSTEM_ADDR) == AM_SYSTEM_ADDR)" */
         } /* end "if ((local_replier.sg_node & ~AM_SYSTEM_ADDR) != AM_SAME_NODE)" */

         if (bGetDirEntry == TRUE)
         {
            /* get "next_station" from function directory */
            status = am_get_dir_entry_idx (func_or_stat, &next_station, card_index);
            if (status == AM_OK)
            {
               if (next_station == AM_UNKNOWN)
               {
                  status = AM_FIN_NOT_REG_ERR;
               } /* end "if (next_station == AM_UNKNOWN)" */
            } /* end "if (status == AM_OK)" */
         } /* end "if (bGetDirEntry == TRUE)" */
      } /* end "if (next_station == AM_UNKNOWN)" */

      /* update "next_station" of "local_replier" */
      local_replier.next_station = next_station;

   } /* end "if (status == AM_OK)" */

   if (status == AM_OK)
   {
      if ((out_msg_size > CMD_MAX_MESSAGE_SIZE) ||
          (in_msg_size > CMD_MAX_MESSAGE_SIZE))
      {
         status = AM_FAILURE;
      } /* end "if ((...))" */
   } /* end "if (status == AM_OK)" */

   if (status == AM_OK)
   {
      if (out_msg_adr == NULL)
      {
         out_msg_size = 0;
      } /* end "if (out_msg_adr == NULL)" */

      if (in_msg_adr == NULL)
      {
         in_msg_dynamic = 1;
      }
      else
      {
         in_msg_dynamic = 0;
      } /* end "if (in_msg_adr == NULL)" */

      /**********************************/
      /* send call over logical channel */
      /**********************************/
      clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

      clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_CALL_REQUEST, card_index); /* index:  0 */
      clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);        /* index:  1 */
      clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, caller_function, card_index);                     /* index:  2 */
      clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, local_replier.sg_node, card_index);               /* index:  3 */
      clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, local_replier.func_or_stat, card_index);          /* index:  4 */
      clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, local_replier.next_station, card_index);          /* index:  5 */
      clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, local_replier.topo_counter, card_index);          /* index:  6 */
      clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &out_msg_size, card_index);                       /* index:  7 */
      clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, in_msg_dynamic, card_index);                      /* index: 11 */
      clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&in_msg_adr, card_index);            /* index: 12 */
      clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &in_msg_size, card_index);                        /* index: 16 */
      clch_channel_send_word_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &reply_timeout, card_index);                      /* index: 20 */
      clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&call_confirm, card_index);          /* index: 22 */
      clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&caller_ref, card_index);            /* index: 26 */
      if (out_msg_size != 0)
      {
         bFirstPackage = FALSE;
         if (clch_channel_send_array_byte_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, out_msg_adr, out_msg_size, card_index) != CLCH_OK)
         {
            status = AM_FAILURE;
         } /* end "if (clch_channel_send_array_byte_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, out_msg_adr, out_msg_size) != CLCH_OK)" */
      }
      else
      {
         bFirstPackage = TRUE;
      } /* end "if (out_msg_size != 0)" */

      if (status == AM_OK)
      {
         
         if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, bFirstPackage, TRUE, card_index) == CLCH_OK)
         {
            /*****************************/
            /* wait for return parameter */
            /*****************************/
            if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_CALL_REQUEST, card_index) == CLCH_OK)
            {
               /************************/
               /* get return parameter */
               /************************/
               clch_receive_word_idx (&temp_result, card_index);
               status = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
               /*************/
               /* check CRC */
               /*************/
               if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
               {
                  status = AM_FAILURE;
               } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
            }
            else
            {
               status = AM_FAILURE;
            } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_CALL_REQUEST) == CLCH_OK)" */
         }
         else
         {
             status = AM_FAILURE;
         } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, bFirstPackage, TRUE, card_index) == CLCH_OK)" */
      } /* end "if (status == AM_OK)" */

      if (status != AM_OK)
      {
         bCallConfirm = TRUE;
      } /* end "if (status != AM_OK)" */

      clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
   }
   else
   {
      bCallConfirm = TRUE;
   } /* end "if (status == AM_OK)" */
   
   if (bCallConfirm == TRUE)
   {
      /***********************************/
      /* perform "call_confirm" function */
      /***********************************/
      call_confirm
      (
         caller_function,
         caller_ref,
         &local_replier,
         in_msg_adr,
         0,
         status
      ); /* end "call_confirm" */

   } /* end "if (bCallConfirm == TRUE)" */

} /* end "am_call_request" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_call_cancel                                             */
/*                                                                            */
/*   INPUT:        caller_function     see TCN documents                      */
/*                 replier             see TCN documents                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              see TCN documents                      */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_call_cancel".               */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_call_cancel (UNSIGNED8 caller_function, const AM_ADDRESS * replier)
{
    return am_call_cancel_idx( caller_function, replier, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_call_cancel_idx (UNSIGNED8 caller_function, const AM_ADDRESS * replier, DG_U8 card_index)
{
AM_RESULT   result;
UNSIGNED16  temp_result;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_announce_device == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_announce_device == FALSE)" */

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_CALL_CANCEL, card_index);  /* index:  0 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);        /* index:  1 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, caller_function, card_index);                     /* index:  2 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier->sg_node, card_index);                    /* index:  3 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier->func_or_stat, card_index);               /* index:  4 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier->next_station, card_index);               /* index:  5 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier->topo_counter, card_index);               /* index:  6 */

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_CALL_CANCEL, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&temp_result, card_index);
         result = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = AM_FAILURE;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = AM_FAILURE;
      } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_CALL_CANCEL) == CLCH_OK)" */
   }
   else
   {
      result = AM_FAILURE;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);

   return (result);

} /* end "am_call_cancel" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_bind_replier                                            */
/*                                                                            */
/*   INPUT:        replier_function    see TCN documents                      */
/*                 receive_confirm     see TCN documents                      */
/*                 reply_confirm       see TCN documents                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              see TCN documents                      */ 
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_bind_replier".              */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_bind_replier
(
   UNSIGNED8            replier_function,
   AM_RECEIVE_CONFIRM   receive_confirm,
   AM_REPLY_CONFIRM     reply_confirm
)
{
    return am_bind_replier_idx(replier_function, receive_confirm, reply_confirm, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_bind_replier_idx (UNSIGNED8          replier_function,
                        AM_RECEIVE_CONFIRM receive_confirm,
                        AM_REPLY_CONFIRM   reply_confirm,
                        DG_U8              card_index)
{
AM_RESULT   result;
UNSIGNED16  temp_result;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_announce_device == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_announce_device == FALSE)" */

   if (receive_confirm == NULL)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (receive_confirm == NULL)" */

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_BIND_REPLIER, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier_function, card_index);
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&receive_confirm, card_index);
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&reply_confirm, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_BIND_REPLIER, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&temp_result, card_index);
         result = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = AM_FAILURE;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = AM_FAILURE;
      } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_BIND_REPLIER) == CLCH_OK)" */
   }
   else
   {
      result = AM_FAILURE;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);

   return (result);

} /* end "am_bind_replier" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_unbind_replier                                          */
/*                                                                            */
/*   INPUT:        replier_function    see TCN documents                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              see TCN documents                      */ 
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_unbind_replier".            */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_unbind_replier (UNSIGNED8 replier_function)
{
    return am_unbind_replier_idx(replier_function, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_unbind_replier_idx (UNSIGNED8 replier_function, DG_U8 card_index)
{
AM_RESULT   result;
UNSIGNED16  temp_result;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_announce_device == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_announce_device == FALSE)" */

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_UNBIND_REPLIER, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier_function, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_UNBIND_REPLIER, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&temp_result, card_index);
         result = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = AM_FAILURE;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = AM_FAILURE;
      } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_UNBIND_REPLIER) == CLCH_OK)" */
   }
   else
   {
      result = AM_FAILURE;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);

   return (result);

} /* end "am_unbind_replier" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_receive_request                                         */
/*                                                                            */
/*   INPUT:        replier_function    see TCN documents                      */
/*                 in_msg_adr          see TCN documents                      */
/*                 in_msg_size         see TCN documents                      */
/*                 replier_ref         see TCN documents                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              see TCN documents                      */ 
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_receive_request".           */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_receive_request
(
   UNSIGNED8   replier_function,
   void *      in_msg_adr,
   UNSIGNED32  in_msg_size,
   void *      replier_ref
)
{
    return am_receive_request_idx(replier_function, in_msg_adr, in_msg_size, replier_ref, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_receive_request_idx (UNSIGNED8    replier_function,
                       void *       in_msg_adr,
                       UNSIGNED32   in_msg_size,
                       void *       replier_ref,
                       DG_U8        card_index)
{
AM_RESULT   result;
UNSIGNED16  temp_result;
UNSIGNED8   in_msg_dynamic;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_announce_device == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_announce_device == FALSE)" */

   if (in_msg_size > CMD_MAX_MESSAGE_SIZE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (in_msg_size > CMD_MAX_MESSAGE_SIZE)" */

   if (in_msg_adr == NULL)
   {
      in_msg_dynamic = 1;
   }
   else
   {
      in_msg_dynamic = 0;
   } /* end "if (in_msg_adr == NULL)" */

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_RECEIVE_REQUEST, card_index); /* index:  0 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);           /* index:  1 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier_function, card_index);                       /* index:  2 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, in_msg_dynamic, card_index);                         /* index:  3 */
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&in_msg_adr, card_index);               /* index:  4 */
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &in_msg_size, card_index);                           /* index:  8 */
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&replier_ref, card_index);              /* index: 12 */

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_RECEIVE_REQUEST, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&temp_result, card_index);
         result = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = AM_FAILURE;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = AM_FAILURE;
      } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_RECEIVE_REQUEST) == CLCH_OK)" */
   }
   else
   {
      result = AM_FAILURE;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);

   return (result);

} /* end "am_receive_request" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_receive_cancel                                          */
/*                                                                            */
/*   INPUT:        replier_function    see TCN documents                      */
/*                 replier_ref         see TCN documents                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              see TCN documents                      */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_receive_cancel".            */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_receive_cancel (UNSIGNED8 replier_function, void * replier_ref)
{
    return am_receive_cancel_idx(replier_function, replier_ref, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_receive_cancel_idx (UNSIGNED8 replier_function, void * replier_ref, DG_U8 card_index)
{
AM_RESULT   result;
UNSIGNED16  temp_result;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_announce_device == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_announce_device == FALSE)" */

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_RECEIVE_CANCEL, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier_function, card_index);
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&replier_ref, card_index);

   if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)
   {
      /*****************************/
      /* wait for return parameter */
      /*****************************/
      if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_RECEIVE_CANCEL, card_index) == CLCH_OK)
      {
         /************************/
         /* get return parameter */
         /************************/
         clch_receive_word_idx (&temp_result, card_index);
         result = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
         /*************/
         /* check CRC */
         /*************/
         if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
         {
            result = AM_FAILURE;
         } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
      }
      else
      {
         result = AM_FAILURE;
      } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_RECEIVE_CANCEL) == CLCH_OK)" */
   }
   else
   {
      result = AM_FAILURE;
   } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, TRUE, TRUE, card_index) == CLCH_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);

   return (result);

} /* end "am_receive_cancel" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_reply_request                                           */
/*                                                                            */
/*   INPUT:        replier_function    see TCN documents                      */
/*                 out_msg_adr         see TCN documents                      */
/*                 out_msg_size        see TCN documents                      */
/*                 replier_ref         see TCN documents                      */
/*                 result              see TCN documents                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              see TCN documents                      */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_receive_cancel".            */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_reply_request
(
   UNSIGNED8   replier_function,
   void *      out_msg_adr,
   UNSIGNED32  out_msg_size,
   void *      replier_ref,
   AM_RESULT   status
)
{
    return am_reply_request_idx(replier_function, out_msg_adr, out_msg_size, replier_ref, status, DG_DEFAULT_IDX);
}

AM_RESULT FunctionDef
   am_reply_request_idx (UNSIGNED8   replier_function,
                     void *      out_msg_adr,
                     UNSIGNED32  out_msg_size,
                     void *      replier_ref,
                     AM_RESULT   status,
                     DG_U8        card_index)
{
   AM_RESULT   result = AM_OK;
   UNSIGNED16  temp_result;
   BOOLEAN1    bFirstPackage;

   if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (cgf_server_status != CGF_SERVER_STATUS_RUN)" */

   if (dg_card_config[card_index].mscf.bDone_am_announce_device == FALSE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (bDone_am_announce_device == FALSE)" */

   if (out_msg_size > CMD_MAX_MESSAGE_SIZE)
   {
      result = AM_FAILURE;
      return (result);
   } /* end "if (out_msg_size > CMD_MAX_MESSAGE_SIZE)" */

   /**********************************/
   /* send call over logical channel */
   /**********************************/
   clch_channel_open_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, card_index);

   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, MCS_FUNCTION_NUMBER_AM_REPLY_REQUEST, card_index);   /* index:  0 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, CLCH_INSTANCE_NUMBER_DEFAULT, card_index);           /* index:  1 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, replier_function, card_index);                       /* index:  2 */
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, &out_msg_size, card_index);                          /* index:  3 */
   clch_channel_send_long_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED32*)&replier_ref, card_index);              /* index:  7 */
   clch_channel_send_byte_idx (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, (UNSIGNED8)status, card_index);                                 /* index: 11 */
   if (out_msg_size != 0)
   {
      bFirstPackage = FALSE;
      if (clch_channel_send_array_byte_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, out_msg_adr, out_msg_size, card_index) != CLCH_OK)
      {
         result = AM_FAILURE;
      } /* end "if (clch_channel_send_array_byte_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, out_msg_adr, out_msg_size) != CLCH_OK)" */
   }
   else
   {
      bFirstPackage = TRUE;
   } /* end "if (out_msg_size != 0)" */

   if (result == AM_OK)
   {
      if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, bFirstPackage, TRUE, card_index) == CLCH_OK)
      {
         /*****************************/
         /* wait for return parameter */
         /*****************************/
         if (clch_wait_for_receive_idx(MCS_RETURN_NUMBER_AM_REPLY_REQUEST, card_index) == CLCH_OK)
         {
            /************************/
            /* get return parameter */
            /************************/
            clch_receive_word_idx (&temp_result, card_index);
            result = (AM_RESULT)temp_result;

#if defined (O_RECEIVE_CRC)
            /*************/
            /* check CRC */
            /*************/
            if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
            {
               result = AM_FAILURE;
            } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif
         }
         else
         {
            result = AM_FAILURE;
         } /* end "if (clch_wait_for_receive(MCS_RETURN_NUMBER_AM_REPLY_REQUEST) == CLCH_OK)" */
      }
      else
      {
         result = AM_FAILURE;
      } /* end "if (clch_channel_send_package_idx(MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA, bFirstPackage, TRUE, card_index) == CLCH_OK)" */
   } /* end "if (result == AM_OK)" */

   clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);

   return (result);

} /* end "am_reply_request" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     am_buffer_free                                             */
/*                                                                            */
/*   INPUT:        in_msg_adr          see TCN documents                      */
/*                 size                see TCN documents                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              see TCN documents                      */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs the "am_buffer_free".               */
/*                                                                            */
/******************************************************************************/

AM_RESULT FunctionDef am_buffer_free (void * in_msg_adr, UNSIGNED32 size)
{
AM_RESULT   result;

   (void)size; /* avoid warning */

   if (in_msg_adr == NULL)
   {
      result = AM_FAILURE;
   }
   else
   {
      cpil_mem_free (in_msg_adr);
      result = AM_OK;
   } /* end "if (in_msg_adr == NULL)" */

   return (result);

} /* end "am_buffer_free" */


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif
