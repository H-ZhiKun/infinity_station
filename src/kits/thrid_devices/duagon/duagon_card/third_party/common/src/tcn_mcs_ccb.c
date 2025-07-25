/* ==========================================================================
 *
 *  Module      :   TCN_MCS_CCB.C
 *
 *  Purpose     :   Callback - i.e. Message Data Confirmation Procedures
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
#include "tcn_mcs_cgf.h"

/******************************************************************************/
/*   MVB Client API                                                           */
/******************************************************************************/
#include "tcn_am.h"

#include "dg_hdio.h"

#include "tcn_mcs_ccb.h"


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
/*   Local Variables                                                          */
/*                                                                            */
/******************************************************************************/



/******************************************************************************/
/*                                                                            */
/*   Internal Function Prototypes                                             */
/*                                                                            */
/******************************************************************************/
void  ccb_process_callback (DG_U8 card_index);
void  ccb_process_call_confirm (DG_U8 card_index);
void  ccb_process_receive_confirm (DG_U8 card_index);
void  ccb_process_reply_confirm (DG_U8 card_index);


/******************************************************************************/
/*                                                                            */
/*   Internal Functions                                                       */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_process_callback                                       */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                      pass 0 for single card systems                        */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function process the callback functions.              */
/*                 The function must be run as a task.                        */
/*                                                                            */
/******************************************************************************/

void ccb_process_callback (DG_U8 card_index)
{
    BOOLEAN1    bError = FALSE;

#if defined (O_PRINT_DEBUG)
    printf ("Process Callback\n");
    printf ("================\n");
    printf ("return_number = %u\n", ccb_return_number);
#endif

    if (dg_card_config[card_index].mscb.bCcbReceiveCrcError == TRUE)
    {
        dg_card_config[card_index].mscb.ccb_return_number = 0;
    } /* end "if (bCcbReceiveCrcError == TRUE)" */

    /********************/
    /* perform callback */
    /********************/
    switch (dg_card_config[card_index].mscb.ccb_return_number)
    {
        case (MCS_RETURN_NUMBER_AM_CALL_CONFIRM):
            if (dg_card_config[card_index].mscb.ccb_call_confirm_function != NULL)
            {
                clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
                ccb_process_call_confirm (card_index);
            }
            else
            {
                bError = TRUE;
            } /* end "if (ccb_call_confirm_function != NULL)" */
            break;
        case (MCS_RETURN_NUMBER_AM_RECEIVE_CONFIRM):
            if (dg_card_config[card_index].mscb.ccb_receive_confirm_function != NULL)
            {
                clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
                ccb_process_receive_confirm (card_index);
            }
            else
            {
                bError = TRUE;
            } /* end "if (ccb_receive_confirm_function != NULL)" */
            break;
        case (MCS_RETURN_NUMBER_AM_REPLY_CONFIRM):
            if (dg_card_config[card_index].mscb.ccb_reply_confirm_function != NULL)
            {
                clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
                ccb_process_reply_confirm (card_index);
            }
            else
            {
                bError = TRUE;
            } /* end "if (ccb_reply_confirm_function != NULL)" */
            break;
        default:
            bError = TRUE;
    } /* end "switch (ccb_return_number)" */

    dg_card_config[card_index].mscb.ccb_package_counter = 0;
    dg_card_config[card_index].mscb.ccb_return_number = 0;
    dg_card_config[card_index].mscb.bCcbFirstPackage = TRUE;
    if (bError == TRUE)
    {
        if (dg_card_config[card_index].mscb.ccb_dynamic_data_base != NULL)
        {
            cpil_mem_free (dg_card_config[card_index].mscb.ccb_dynamic_data_base);
        } /* end "if (ccb_dynamic_data_base != NULL)" */
        clch_channel_close (MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA);
    } /* end "if (bError == TRUE)" */

} /* end "ccb_process_callback" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_process_call_confirm                                   */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                      pass 0 for single card systems                        */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function process a call confirm.                      */
/*                                                                            */
/******************************************************************************/

void ccb_process_call_confirm (DG_U8 card_index)
{

   /**************************************/
   /* perform "AM_CALL_CONFIRM" function */
   /**************************************/
   dg_card_config[card_index].mscb.ccb_call_confirm_function
   (
      dg_card_config[card_index].mscb.ccb_caller_function,
      dg_card_config[card_index].mscb.ccb_caller_ref,
      &dg_card_config[card_index].mscb.ccb_replier,
      dg_card_config[card_index].mscb.ccb_in_msg_adr,
      dg_card_config[card_index].mscb.ccb_in_msg_size,
      dg_card_config[card_index].mscb.ccb_status
   ); /* end "ccb_call_confirm_function" */

} /* end "ccb_process_call_confirm" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_process_receive_confirm                                */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                      pass 0 for single card systems                        */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function process a receive confirm.                   */
/*                                                                            */
/******************************************************************************/

void ccb_process_receive_confirm (DG_U8 card_index)
{

   /*****************************************/
   /* perform "AM_RECEIVE_CONFIRM" function */
   /*****************************************/
   dg_card_config[card_index].mscb.ccb_receive_confirm_function
   (
      dg_card_config[card_index].mscb.ccb_replier_function,
      &dg_card_config[card_index].mscb.ccb_caller,
      dg_card_config[card_index].mscb.ccb_in_msg_adr,
      dg_card_config[card_index].mscb.ccb_in_msg_size,
      dg_card_config[card_index].mscb.ccb_replier_ref
   ); /* end "ccb_receive_confirm_function" */

} /* end "ccb_process_receive_confirm" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_process_reply_confirm                                  */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                      pass 0 for single card systems                        */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function process a reply confirm.                     */
/*                                                                            */
/******************************************************************************/

void ccb_process_reply_confirm (DG_U8 card_index)
{

   /***************************************/
   /* perform "AM_REPLY_CONFIRM" function */
   /***************************************/
   dg_card_config[card_index].mscb.ccb_reply_confirm_function
   (
      dg_card_config[card_index].mscb.ccb_replier_function,
      dg_card_config[card_index].mscb.ccb_replier_ref
   ); /* end "ccb_reply_confirm_function" */

} /* end "ccb_process_reply_confirm" */


/******************************************************************************/
/*                                                                            */
/*   External Functions                                                       */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_init                                                   */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                                 pass 0 for single card systems             */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function initialises local variables.                 */
/*                                                                            */
/******************************************************************************/
void ccb_init(void)
{
    ccb_init_idx(DG_DEFAULT_IDX);
}

void ccb_init_idx (DG_U8 card_index)
{

   dg_card_config[card_index].mscb.ccb_package_counter = 0;
   dg_card_config[card_index].mscb.ccb_return_number = 0;
   dg_card_config[card_index].mscb.bCcbFirstPackage = TRUE;
   dg_card_config[card_index].mscb.ccb_dynamic_data_base = NULL;

} /* end "ccb_init" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_clear_callback                                         */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                                 pass 0 for single card systems             */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function clears a callback.                           */
/*                                                                            */
/******************************************************************************/
void ccb_clear_callback(void)
{
    ccb_clear_callback_idx(DG_DEFAULT_IDX);
}
void ccb_clear_callback_idx (DG_U8 card_index)
{

#if !defined (O_OS_SINGLE)
    cpil_mutex_take (CPIL_MUTEX_CALLBACK);
#endif

    if (dg_card_config[card_index].mscb.ccb_return_number != 0)
    {
        dg_card_config[card_index].mscb.ccb_package_counter = 0;
        dg_card_config[card_index].mscb.ccb_return_number = 0;
        dg_card_config[card_index].mscb.bCcbFirstPackage = TRUE;
        if (dg_card_config[card_index].mscb.ccb_dynamic_data_base != NULL)
        {
            cpil_mem_free (dg_card_config[card_index].mscb.ccb_dynamic_data_base);
        } /* end "if (ccb_dynamic_data_base != NULL)" */
    } /* end "if (ccb_return_number != 0) */

#if !defined (O_OS_SINGLE)
    cpil_mutex_give (CPIL_MUTEX_CALLBACK);
#endif

} /* end "ccb_clear_callback" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_call_confirm                                           */
/*                                                                            */
/*   INPUT:        package_size                                               */
/*                 card_index:    Card to use in multi-card system            */
/*                                pass 0 for single card systems              */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: ready         TRUE (ready), FALSE (busy)                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function process a call confirm.                      */
/*                                                                            */
/******************************************************************************/
BOOLEAN1 ccb_call_confirm (UNSIGNED8 package_size)
{
    return ccb_call_confirm_idx (package_size, DG_DEFAULT_IDX);
}
BOOLEAN1 ccb_call_confirm_idx (UNSIGNED8 package_size, DG_U8 card_index)
{
UNSIGNED32  counter;
UNSIGNED8   message_dummy;
UNSIGNED8   status_temp;

   dg_card_config[card_index].mscb.ccb_package_counter++;

   if (dg_card_config[card_index].mscb.bCcbFirstPackage == TRUE)
   {
      dg_card_config[card_index].mscb.ccb_return_number = MCS_RETURN_NUMBER_AM_CALL_CONFIRM;
      dg_card_config[card_index].mscb.bCcbFirstPackage = FALSE;
      dg_card_config[card_index].mscb.bCcbReceiveCrcError = FALSE;

      /*********************************/
      /* receive function package data */
      /*********************************/
      clch_receive_long_idx ((UNSIGNED32*)&dg_card_config[card_index].mscb.ccb_call_confirm_function, card_index);
      clch_receive_byte_idx (&dg_card_config[card_index].mscb.ccb_caller_function, card_index);
      clch_receive_long_idx ((UNSIGNED32*)&dg_card_config[card_index].mscb.ccb_caller_ref, card_index);
      clch_receive_byte_idx (&(dg_card_config[card_index].mscb.ccb_replier.sg_node), card_index);
      clch_receive_byte_idx (&(dg_card_config[card_index].mscb.ccb_replier.func_or_stat), card_index);
      clch_receive_byte_idx (&(dg_card_config[card_index].mscb.ccb_replier.next_station), card_index);
      clch_receive_byte_idx (&(dg_card_config[card_index].mscb.ccb_replier.topo_counter), card_index);
      clch_receive_long_idx ((UNSIGNED32*)&dg_card_config[card_index].mscb.ccb_in_msg_adr, card_index);
      clch_receive_long_idx (&dg_card_config[card_index].mscb.ccb_in_msg_size, card_index);
      clch_receive_byte_idx ( &status_temp, card_index);
      dg_card_config[card_index].mscb.ccb_status = (AM_RESULT)status_temp;
      
      dg_card_config[card_index].mscb.ccb_in_msg_size_counter = dg_card_config[card_index].mscb.ccb_in_msg_size;

      #if defined (O_PRINT_DEBUG)
         printf ("AM_CALL_CONFIRM()\n");
         printf ("=================\n");
         printf ("p_call_confirm_function = 0x%08lX\n", dg_card_config[card_index].mscb.ccb_call_confirm_function);
         printf ("caller_function         = %u\n", dg_card_config[card_index].mscb.ccb_caller_function);
         printf ("caller_ref              = 0x%08lX\n", dg_card_config[card_index].mscb.ccb_caller_ref);
         printf ("replier.sg_node         = %u\n", dg_card_config[card_index].mscb.ccb_replier.sg_node);
         printf ("replier.func_or_stat    = %u\n", dg_card_config[card_index].mscb.ccb_replier.func_or_stat);
         printf ("replier.next_station    = %u\n", dg_card_config[card_index].mscb.ccb_replier.next_station);
         printf ("replier.topo_counter    = %u\n", dg_card_config[card_index].mscb.ccb_replier.topo_counter);
         printf ("in_msg_adr              = 0x%08lX\n", dg_card_config[card_index].mscb.ccb_in_msg_adr);
         printf ("in_msg_size             = %lu\n", dg_card_config[card_index].mscb.ccb_in_msg_size);
         printf ("status                  = %u\n", dg_card_config[card_index].mscb.ccb_status);
      #endif

#if defined (O_RECEIVE_CRC)
      /*************/
      /* check CRC */
      /*************/
      if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
      {
         dg_card_config[card_index].mscb.bCcbReceiveCrcError = TRUE;
         return (FALSE);
      } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif

      /***********************************/
      /* check, if "in_msg" is available */
      /***********************************/
      if (dg_card_config[card_index].mscb.ccb_in_msg_size > 0)
      {
         /***************************/
         /* allocate dynamic memory */
         /* - package data (in_msg) */
         /***************************/
         if (dg_card_config[card_index].mscb.ccb_in_msg_adr == NULL)
         {
            dg_card_config[card_index].mscb.ccb_dynamic_data_base = (void*)cpil_mem_alloc(dg_card_config[card_index].mscb.ccb_in_msg_size);
            if (dg_card_config[card_index].mscb.ccb_dynamic_data_base == NULL)
            {
               dg_card_config[card_index].mscb.ccb_status = AM_NO_LOC_MEM_ERR;
            } /* end "if (ccb_dynamic_data == NULL)" */

            dg_card_config[card_index].mscb.ccb_in_msg_adr = (void*)dg_card_config[card_index].mscb.ccb_dynamic_data_base;
            dg_card_config[card_index].mscb.ccb_data_pointer = dg_card_config[card_index].mscb.ccb_dynamic_data_base;
         }
         else
         {
            dg_card_config[card_index].mscb.ccb_data_pointer = dg_card_config[card_index].mscb.ccb_in_msg_adr;
         } /* end "if (ccb_in_msg_adr == NULL)" */

         return (FALSE);

      } /* end "if (ccb_in_msg_size > 0)" */

   } /* end "if (bCcbFirstPackage == TRUE)" */

   /*****************************************/
   /* receive dynamic package data (in_msg) */
   /*****************************************/
   if (dg_card_config[card_index].mscb.ccb_in_msg_size > 0)
   {
      dg_card_config[card_index].mscb.ccb_in_msg_size_counter = dg_card_config[card_index].mscb.ccb_in_msg_size_counter - package_size;

      /* receive "in_msg" */
      if (dg_card_config[card_index].mscb.ccb_in_msg_adr == NULL)
      {
         counter = 0;
         do
         {
            clch_receive_byte_idx (&message_dummy, card_index);
            counter++;
         } while (counter < package_size);
      }
      else
      {
         clch_receive_array_byte_idx (dg_card_config[card_index].mscb.ccb_data_pointer, package_size, card_index);
         dg_card_config[card_index].mscb.ccb_data_pointer += package_size;
      } /* end "if (ccb_in_msg_adr == NULL)" */

#if defined (O_RECEIVE_CRC)
      /*************/
      /* check CRC */
      /*************/
      if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
      {
         dg_card_config[card_index].mscb.bCcbReceiveCrcError = TRUE;
      } /* end "if (clch_receive_check_crc_idx(card_index) != SLCH_OK)" */
#endif

      if (dg_card_config[card_index].mscb.ccb_in_msg_size_counter > 0)
      {
         return (FALSE);
      } /* end "if (ccb_in_msg_size_counter > 0)" */

   } /* end "if (ccb_in_msg_size > 0)" */

   /********************/
   /* perform callback */
   /********************/
   ccb_process_callback (card_index);

   return (TRUE);

} /* end "ccb_call_confirm" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_receive_confirm                                        */
/*                                                                            */
/*   INPUT:        package_size                                               */
/*                 card_index:    Card to use in multi-card system            */
/*                                pass 0 for single card systems              */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: ready         TRUE (ready), FALSE (busy)                   */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function process a receive confirm.                   */
/*                                                                            */
/******************************************************************************/
BOOLEAN1    ccb_receive_confirm (UNSIGNED8 package_size)
{
    return ccb_receive_confirm_idx(package_size, DG_DEFAULT_IDX);
}
BOOLEAN1 ccb_receive_confirm_idx (UNSIGNED8 package_size, DG_U8 card_index)
{
UNSIGNED32  counter;
UNSIGNED8   message_dummy;

   dg_card_config[card_index].mscb.ccb_package_counter++;

   if (dg_card_config[card_index].mscb.bCcbFirstPackage == TRUE)
   {
      dg_card_config[card_index].mscb.ccb_return_number = MCS_RETURN_NUMBER_AM_RECEIVE_CONFIRM;
      dg_card_config[card_index].mscb.bCcbFirstPackage = FALSE;
      dg_card_config[card_index].mscb.bCcbReceiveCrcError = FALSE;

      /*********************************/
      /* receive function package data */
      /*********************************/
      clch_receive_long_idx ((UNSIGNED32*)&dg_card_config[card_index].mscb.ccb_receive_confirm_function, card_index);
      clch_receive_byte_idx (&dg_card_config[card_index].mscb.ccb_replier_function, card_index);
      clch_receive_byte_idx (&(dg_card_config[card_index].mscb.ccb_caller.sg_node), card_index);
      clch_receive_byte_idx (&(dg_card_config[card_index].mscb.ccb_caller.func_or_stat), card_index);
      clch_receive_byte_idx (&(dg_card_config[card_index].mscb.ccb_caller.next_station), card_index);
      clch_receive_byte_idx (&(dg_card_config[card_index].mscb.ccb_caller.topo_counter), card_index);
      clch_receive_long_idx ((UNSIGNED32*)&dg_card_config[card_index].mscb.ccb_in_msg_adr, card_index);
      clch_receive_long_idx (&dg_card_config[card_index].mscb.ccb_in_msg_size, card_index);
      clch_receive_long_idx ((UNSIGNED32*)&dg_card_config[card_index].mscb.ccb_replier_ref, card_index);

      dg_card_config[card_index].mscb.ccb_in_msg_size_counter = dg_card_config[card_index].mscb.ccb_in_msg_size;

      #if defined (O_PRINT_DEBUG)
         printf ("AM_RECEIVE_CONFIRM()\n");
         printf ("====================\n");
         printf ("p_receive_confirm_function = 0x%08lX\n", dg_card_config[card_index].mscb.ccb_receive_confirm_function);
         printf ("replier_function           = %u\n", dg_card_config[card_index].mscb.ccb_replier_function);
         printf ("caller.sg_node             = %u\n", dg_card_config[card_index].mscb.ccb_caller.sg_node);
         printf ("caller.func_or_stat        = %u\n", dg_card_config[card_index].mscb.ccb_caller.func_or_stat);
         printf ("caller.next_station        = %u\n", dg_card_config[card_index].mscb.ccb_caller.next_station);
         printf ("caller.topo_counter        = %u\n", dg_card_config[card_index].mscb.ccb_caller.topo_counter);
         printf ("in_msg_adr                 = 0x%08lX\n", dg_card_config[card_index].mscb.ccb_in_msg_adr);
         printf ("in_msg_size                = %lu\n", dg_card_config[card_index].mscb.ccb_in_msg_size);
         printf ("replier_ref                = 0x%08lX\n", dg_card_config[card_index].mscb.ccb_replier_ref);
      #endif

#if defined (O_RECEIVE_CRC)
      /*************/
      /* check CRC */
      /*************/
      if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
      {
         dg_card_config[card_index].mscb.bCcbReceiveCrcError = TRUE;
         return (FALSE);
      } /* end "if (clch_receive_check_crc_idx(card_index) != CLCH_OK)" */
#endif

      /***********************************/
      /* check, if "in_msg" is available */
      /***********************************/
      if (dg_card_config[card_index].mscb.ccb_in_msg_size > 0)
      {
         /***************************/
         /* allocate dynamic memory */
         /* - package data (in_msg) */
         /***************************/
         if (dg_card_config[card_index].mscb.ccb_in_msg_adr == NULL)
         {
            dg_card_config[card_index].mscb.ccb_dynamic_data_base = (void*)cpil_mem_alloc(dg_card_config[card_index].mscb.ccb_in_msg_size);
            if (dg_card_config[card_index].mscb.ccb_dynamic_data_base == NULL)
            {
               dg_card_config[card_index].mscb.ccb_status = AM_NO_LOC_MEM_ERR;
            } /* end "if (ccb_dynamic_data == NULL)" */

            dg_card_config[card_index].mscb.ccb_in_msg_adr = (void*)dg_card_config[card_index].mscb.ccb_dynamic_data_base;
            dg_card_config[card_index].mscb.ccb_data_pointer = dg_card_config[card_index].mscb.ccb_dynamic_data_base;
         }
         else
         {
            dg_card_config[card_index].mscb.ccb_data_pointer =
                    dg_card_config[card_index].mscb.ccb_in_msg_adr;
         } /* end "if (ccb_in_msg_adr == NULL)" */

         return (FALSE);

      } /* end "if (ccb_in_msg_size > 0)" */
   } /* end "if (bCcbFirstPackage == TRUE)" */

   /*****************************************/
   /* receive dynamic package data (in_msg) */
   /*****************************************/
   if (dg_card_config[card_index].mscb.ccb_in_msg_size > 0)
   {
      dg_card_config[card_index].mscb.ccb_in_msg_size_counter =
              dg_card_config[card_index].mscb.ccb_in_msg_size_counter - package_size;

      /* receive "in_msg" */
      if (dg_card_config[card_index].mscb.ccb_in_msg_adr == NULL)
      {
         counter = 0;
         do
         {
            clch_receive_byte_idx (&message_dummy, card_index);
            counter++;
         } while (counter < package_size);
      }
      else
      {
         clch_receive_array_byte_idx (dg_card_config[card_index].mscb.ccb_data_pointer, package_size, card_index);
         dg_card_config[card_index].mscb.ccb_data_pointer += package_size;
      } /* end "if (ccb_in_msg_adr == NULL)" */

#if defined (O_RECEIVE_CRC)
      /*************/
      /* check CRC */
      /*************/
      if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
      {
         dg_card_config[card_index].mscb.bCcbReceiveCrcError = TRUE;
      } /* end "if (clch_receive_check_crc_idx(card_index) != SLCH_OK)" */
#endif

      if (dg_card_config[card_index].mscb.ccb_in_msg_size_counter > 0)
      {
         return (FALSE);
      } /* end "if (ccb_in_msg_size_counter > 0)" */

   } /* end "if (ccb_in_msg_size > 0)" */

   /********************/
   /* perform callback */
   /********************/
   ccb_process_callback (card_index);

   return (TRUE);

} /* end "ccb_receive_confirm" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_reply_confirm                                          */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                      pass 0 for single card systems                        */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function process a reply confirm.                     */
/*                                                                            */
/******************************************************************************/

void ccb_reply_confirm(void)
{
    ccb_reply_confirm_idx(DG_DEFAULT_IDX);
}

void ccb_reply_confirm_idx (DG_U8 card_index)
{

   dg_card_config[card_index].mscb.ccb_package_counter++;
   dg_card_config[card_index].mscb.ccb_return_number = MCS_RETURN_NUMBER_AM_REPLY_CONFIRM;

   /*********************************/
   /* receive function package data */
   /*********************************/
   clch_receive_long_idx ((UNSIGNED32*)&dg_card_config[card_index].mscb.ccb_reply_confirm_function, card_index);
   clch_receive_byte_idx (&dg_card_config[card_index].mscb.ccb_replier_function, card_index);
   clch_receive_long_idx ((UNSIGNED32*)&dg_card_config[card_index].mscb.ccb_replier_ref, card_index);

   #if defined (O_PRINT_DEBUG)
      printf ("AM_REPLY_CONFIRM()\n");
      printf ("==================\n");
      printf ("p_reply_confirm_function = 0x%08lX\n", dg_card_config[card_index].mscb.ccb_reply_confirm_function);
      printf ("replier_function         = %u\n", dg_card_config[card_index].mscb.ccb_replier_function);
      printf ("replier_ref              = 0x%08lX\n", dg_card_config[card_index].mscb.ccb_replier_ref);
   #endif

   #if defined (O_RECEIVE_CRC)
      /*************/
      /* check CRC */
      /*************/
      if (clch_receive_check_crc_idx(card_index) != CLCH_OK)
      {
         dg_card_config[card_index].mscb.bCcbReceiveCrcError = TRUE;
      } /* end "if (clch_receive_check_crc_idx(card_index) != SLCH_OK)" */
   #endif

   /********************/
   /* perform callback */
   /********************/
   ccb_process_callback (card_index);

} /* end "ccb_reply_confirm" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     ccb_get_package_counter                                    */
/*                                                                            */
/*   INPUT:        -    card_index Card to use in multi-card system           */
/*                      pass 0 for single card systems                        */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function gets the number of received                  */
/*                 callback packages.                                         */
/*                                                                            */
/******************************************************************************/
UNSIGNED32  ccb_get_package_counter (void)
{
    return ccb_get_package_counter_idx(DG_DEFAULT_IDX);
}
UNSIGNED32 ccb_get_package_counter_idx (DG_U8 card_index)
{

   return (dg_card_config[card_index].mscb.ccb_package_counter);

} /* end "ccb_get_package_counter" */


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif
