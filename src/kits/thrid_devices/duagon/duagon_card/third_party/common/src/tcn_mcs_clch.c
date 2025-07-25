/* ==========================================================================
 *
 *  Module      :   TCN_MCS_CLCH.C
 *
 *  Purpose     :   Logical Channel Communication
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
#if defined (O_PRINT_DEBUG) || defined (O_PRINT_DEBUG_CH)
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
/*   Receive Status                                                           */
/******************************************************************************/
#define CLCH_RECEIVE_STATUS_REPEAT  0
#define CLCH_RECEIVE_STATUS_OK      1
#define CLCH_RECEIVE_STATUS_ERROR   2


/******************************************************************************/
/*                                                                            */
/*   Global Variables                                                         */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*   Local Variables                                                          */
/*                                                                            */
/******************************************************************************/

static UNSIGNED8 * const
p_channel_send_package_header[DG_NR_OF_CARDS_DEFINED][MCS_CLIENT_CHANNEL_NUMBER_MAX]
       = DG_CHANNEL_SEND_PACKAGE_HEADER_INIT;

static UNSIGNED8 * const
p_channel_send_package_data[DG_NR_OF_CARDS_DEFINED][MCS_CLIENT_CHANNEL_NUMBER_MAX]
       = DG_CHANNEL_SEND_PACKAGE_DATA_INIT;

/******************************************************************************/
/*                                                                            */
/*   External Functions                                                       */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_init                                                  */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function initialises the local variables              */
/*                 of the logical channels.                                   */
/*                                                                            */
/******************************************************************************/

void clch_init (void)
{
    clch_init_idx(DG_DEFAULT_IDX);
}
void clch_init_idx (DG_U8 card_index)
{
UNSIGNED8   channel_number;

   /***************************************************************************/
   /*   Channel Package (receive)                                             */
   /***************************************************************************/
   channel_number = 0;
   do
   {
      dg_card_config[card_index].msch.bChannelReceiveFirstPackage[channel_number] = (BOOLEAN1)TRUE;
      dg_card_config[card_index].msch.channel_receive_return_number[channel_number] = (UNSIGNED8)0;

      channel_number++;

   } while (channel_number < MCS_SERVER_CHANNEL_NUMBER_MAX);

} /* end "clch_init" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_channel_open                                          */
/*                                                                            */
/*   INPUT:        channel_number                                             */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function opens a logical channel.                     */
/*                                                                            */
/******************************************************************************/

void clch_channel_open (UNSIGNED16 channel_number)
{
    clch_channel_open_idx(channel_number, DG_DEFAULT_IDX);
}
void clch_channel_open_idx (UNSIGNED16 channel_number, DG_U8 card_index)
{

   #if defined (O_PRINT_DEBUG_CH)
      printf("|C%dA>", channel_number);
   #endif

   #if !defined (O_OS_SINGLE)
      cpil_mutex_take (channel_number);
   #endif

   #if defined (O_PRINT_DEBUG_CH)
      printf("|C%dB>", channel_number);
   #endif

   dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number] = \
      p_channel_send_package_data[card_index][channel_number];

} /* end "clch_channel_open" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_channel_close                                         */
/*                                                                            */
/*   INPUT:        channel_number                                             */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function closes a logical channel.                    */
/*                                                                            */
/******************************************************************************/

void clch_channel_close (UNSIGNED16 channel_number)
{

   #if defined (O_OS_SINGLE)
      (void) channel_number; /* avoid warning */
   #endif

   #if defined (O_PRINT_DEBUG_CH)
      printf("<H%d|", channel_number);
   #endif

   #if !defined (O_OS_SINGLE)
      cpil_mutex_give (CPIL_MUTEX_HARDWARE_CHANNEL);
   #endif

   #if defined (O_PRINT_DEBUG_CH)
      printf("<C%d|", channel_number);
   #endif

   #if !defined (O_OS_SINGLE)
      cpil_mutex_give (channel_number);
   #endif

} /* end "clch_channel_close" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_channel_receive                                       */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       p_return_number          MCS_RETURN_NUMBER_...             */
/*                 p_package_size           package data size                 */
/*                                                                            */
/*   RETURN VALUE: channel_receive_status   CLCH_RECEIVE_STATUS_...           */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function receives data from the logical channels.     */
/*                 NOTE:                                                      */
/*                 This function must be run as a cyclic process.             */
/*                                                                            */
/******************************************************************************/

UNSIGNED8 clch_channel_receive (UNSIGNED8 *p_return_number, UNSIGNED8 *p_package_size)
{
    return clch_channel_receive_idx(p_return_number, p_package_size, DG_DEFAULT_IDX);
}
UNSIGNED8 clch_channel_receive_idx (
        UNSIGNED8 *p_return_number,
        UNSIGNED8 *p_package_size,
        DG_U8 card_index)
{
UNSIGNED8   start_delimiter;
UNSIGNED8   channel_number;
UNSIGNED8   package_size;
UNSIGNED8   package_crc;
UNSIGNED8   device_status;
UNSIGNED8   instance_number;

   *p_return_number = 0;
   *p_package_size  = 0;

   #if defined (O_RECEIVE_CRC)
      /* clear receive CRC */
      dg_card_config[card_index].msch.channel_receive_crc = 0;
   #endif


   /************************************/
   /* receive and check package header */
   /************************************/

   /* wait until package header is received */
   if (cpil_wait_for_hardware_receive_idx(5, card_index) != CPIL_OK)
   {
      return(CLCH_RECEIVE_STATUS_ERROR);
   } /* end "if (cpil_wait_for_hardware_receive_idx(5, card_index) != CPIL_OK)" */

   /* 1. receive "start_delimiter" */
   clch_receive_byte_idx (&start_delimiter, card_index);
   if (start_delimiter != (UNSIGNED8)MCS_SERVER_CHANNEL_HEADER_START_DELIMITER)
   {
      #if defined (O_PRINT_DEBUG)
         printf ("[ERROR: start_delimiter=0x%02X]", start_delimiter);
      #endif
      return(CLCH_RECEIVE_STATUS_REPEAT);
   } /* end "if (start_delimiter != (UNSIGNED8)MCS_SERVER_CHANNEL_HEADER_START_DELIMITER)" */

   /* 2. receive "channel_number" */
   clch_receive_byte_idx (&channel_number, card_index);

   /* 3. receive "package_size" */
   clch_receive_byte_idx (&package_size, card_index);
   *p_package_size = package_size;

   /* 4. receive "package_crc" */
   clch_receive_byte_idx (&package_crc, card_index);

   /* 5. receive "device_status" */
   clch_receive_byte_idx (&device_status, card_index);
   if (device_status != 0x00)
   {
      #if defined (O_PRINT_DEBUG)
         printf ("[ERROR: device_status=%d]", device_status);
      #endif
      cgf_put_device_status (device_status, card_index);
   } /* end "if (device_status != 0x00)" */

   #if defined (O_PRINT_DEBUG)
      printf ("[channel_number=%d]", channel_number);
      printf ("[package_size=%d]", package_size);
      printf ("[package_CRC=0x%02X]", package_crc);
      printf ("[device_status=%d]", device_status);
   #endif


   /************************/
   /* receive package data */
   /************************/

   /* wait until package data is received */
   if (cpil_wait_for_hardware_receive_idx(package_size, card_index) != CPIL_OK)
   {
      return(CLCH_RECEIVE_STATUS_ERROR);
   } /* end "if (cpil_wait_for_hardware_receive_idx(package_size, card_index) != CPIL_OK)" */

   #if defined (O_PRINT_DEBUG)
      printf ("[package-received]");
   #endif

   if (dg_card_config[card_index].msch.bChannelReceiveFirstPackage[channel_number] == TRUE)
   {
      /* 1. receive "return_number" */
      clch_receive_byte_idx (&dg_card_config[card_index].msch.channel_receive_return_number[channel_number], card_index);
      #if defined (O_PRINT_DEBUG)
         printf ("return number = %d\r\n", dg_card_config[card_index].msch.channel_receive_return_number[channel_number]);
      #endif
      *p_return_number = dg_card_config[card_index].msch.channel_receive_return_number[channel_number];

      /* 2. receive "instance_number" */
      clch_receive_byte_idx (&instance_number, card_index);

   } /* end "if (bChannelReceiveFirstPackage[channel_number] == FALSE)" */

#ifdef O_DEBUG_FCT_NR
if (dg_card_config[card_index].msch.bChannelReceiveFirstPackage[channel_number] == TRUE)
{
    if (((*p_return_number >= 20) && (*p_return_number < 30)) || ((*p_return_number >= 120) && (*p_return_number < 170)))
    {
        printf("c%d,", *p_return_number);
    }
}
else
{
    printf("d,");
}
#endif /* O_DEBUG_FCT_NR */


   /**********************/
   /* processes a return */
   /**********************/

   if (channel_number == MCS_SERVER_CHANNEL_NUMBER_RETURN)
   {
      /*******************/
      /* Function Return */
      /*******************/
      return(CLCH_RECEIVE_STATUS_OK);
   }
   else if (channel_number == MCS_SERVER_CHANNEL_NUMBER_CALLBACK)
   {
      /*******************************************************/
      /* Message Data Callback                               */
      /* NOTE:                                               */
      /* If a callback is finished, the logical channel      */
      /* "MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA" is closed. */
      /*******************************************************/
      switch (dg_card_config[card_index].msch.channel_receive_return_number[MCS_SERVER_CHANNEL_NUMBER_CALLBACK])
      {
         case (MCS_RETURN_NUMBER_AM_CALL_CONFIRM):
            /* call confirm */
            dg_card_config[card_index].msch.bChannelReceiveFirstPackage[MCS_SERVER_CHANNEL_NUMBER_CALLBACK] = ccb_call_confirm_idx (package_size, card_index);
            return(CLCH_RECEIVE_STATUS_OK);
            break;
         case (MCS_RETURN_NUMBER_AM_RECEIVE_CONFIRM):
            /* receive confirm */
            dg_card_config[card_index].msch.bChannelReceiveFirstPackage[MCS_SERVER_CHANNEL_NUMBER_CALLBACK] = ccb_receive_confirm_idx (package_size, card_index);
            return(CLCH_RECEIVE_STATUS_OK);
            break;
         case (MCS_RETURN_NUMBER_AM_REPLY_CONFIRM):
            /* reply confirm */
            ccb_reply_confirm_idx (card_index);
            return(CLCH_RECEIVE_STATUS_OK);
            break;
         default: break;
      } /* end "switch (channel_receive_return_number[MCS_SERVER_CHANNEL_NUMBER_CALLBACK])" */
   }
   else if (channel_number == MCS_SERVER_CHANNEL_NUMBER_LIFESIGN)
   {
      /*************/
      /* Life Sign */
      /*************/
      /* do nothing */
      if (device_status != GF_DEVICE_STATUS_OK)
      {
         return(CLCH_RECEIVE_STATUS_ERROR);
      } /* end "if (device_status != GF_DEVICE_STATUS_OK)" */
   } /* end "if (channel_number == MCS_SERVER_CHANNEL_NUMBER_RETURN)" */


   return(CLCH_RECEIVE_STATUS_REPEAT);

} /* end "clch_channel_receive" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_wait_for_receive                                      */
/*                                                                            */
/*   INPUT:        return_number    MCS_RETURN_NUMBER_...                     */
/*                                  0 - don't check return number             */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result           CLCH_OK, CLCH_ERROR                       */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function waits until the data a ready to receive.     */
/*                                                                            */
/******************************************************************************/

CLCH_RESULT clch_wait_for_receive (UNSIGNED8 return_number)
{
    return clch_wait_for_receive_idx(return_number, DG_DEFAULT_IDX);
}
CLCH_RESULT clch_wait_for_receive_idx (UNSIGNED8 return_number, DG_U8 card_index)
{
UNSIGNED8   channel_receive_status;
UNSIGNED8   received_return_number;
UNSIGNED8   received_package_size;

    dg_card_config[card_index].msch.clch_return_number = return_number;
    for (;;)
    {
        channel_receive_status = \
            clch_channel_receive_idx (&received_return_number, &received_package_size, card_index);

        if (channel_receive_status == CLCH_RECEIVE_STATUS_OK)
        {
            /* REWORK:                                                        */
            /* MVB server may return MCS_RETURN_NUMBER_LP_PUT_DATASET         */
            /* instead of MCS_RETURN_NUMBER_LP_GET_DATASET;                   */
            /* i.e. accept both return number:                                */
            /* - MCS_RETURN_NUMBER_LP_GET_DATASET (expected return number)    */
            /* - MCS_RETURN_NUMBER_LP_PUT_DATASET (alternative return number, */
            /*   due to erroneous MVB server)                                 */
            if (return_number == MCS_RETURN_NUMBER_LP_GET_DATASET)
            {
                if ((received_return_number != MCS_RETURN_NUMBER_LP_GET_DATASET) && \
                    (received_return_number != MCS_RETURN_NUMBER_LP_PUT_DATASET))
                {
#ifdef O_DEBUG_RETURN_NR
                printf("ERROR: (1) return_number, %d, %d\n", return_number, received_return_number);
#endif
                    return (CLCH_ERROR);
                } /* if ((...)) */
                return (CLCH_OK);
            } /* if (return_number == MCS_RETURN_NUMBER_LP_GET_DATASET) */

            if (return_number != 0)
            {
                if (received_return_number != return_number)
                {
#ifdef O_DEBUG_RETURN_NR
                    printf("ERROR: (2) return_number, %d, %d\n", return_number, received_return_number);
#endif
                    return (CLCH_ERROR);
                } /* if (received_return_number != return_number) */
            } /* if (return_number != 0) */

            return (CLCH_OK);

        } /* end "if (channel_receive_status == CLCH_RECEIVE_STATUS_OK)" */

        if (channel_receive_status == CLCH_RECEIVE_STATUS_ERROR)
        {
            return (CLCH_ERROR);
        } /* end "if (channel_receive_status == CLCH_RECEIVE_STATUS_ERROR)" */

    } /* end "for (;;)" */

    return 0;

} /* end "clch_wait_for_receive" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_wait_for_receive_dynamic                              */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       package_size                                               */
/*                                                                            */
/*   RETURN VALUE: CLCH_RESULT      CLCH_OK, CLCH_ERROR                       */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function waits until the data a ready to receive.     */
/*                                                                            */
/******************************************************************************/

CLCH_RESULT clch_wait_for_receive_dynamic (UNSIGNED8 * package_size)
{
    return clch_wait_for_receive_dynamic_idx(package_size,DG_DEFAULT_IDX);
}
CLCH_RESULT clch_wait_for_receive_dynamic_idx (UNSIGNED8 * package_size, DG_U8 card_index)
{
UNSIGNED8   channel_receive_status;
UNSIGNED8   received_return_number;
UNSIGNED8   received_package_size;

   dg_card_config[card_index].msch.clch_return_number = 0;

   dg_card_config[card_index].msch.bChannelReceiveFirstPackage[MCS_SERVER_CHANNEL_NUMBER_RETURN] = FALSE;

   for (;;)
   {
      channel_receive_status = \
         clch_channel_receive_idx (&received_return_number, &received_package_size, card_index);

      if (channel_receive_status == CLCH_RECEIVE_STATUS_OK)
      {
         *package_size = received_package_size;
         dg_card_config[card_index].msch.bChannelReceiveFirstPackage[MCS_SERVER_CHANNEL_NUMBER_RETURN] = TRUE;
         return (CLCH_OK);
      } /* end "if (channel_receive_status == CLCH_RECEIVE_STATUS_OK)" */

      if (channel_receive_status == CLCH_RECEIVE_STATUS_ERROR)
      {
         dg_card_config[card_index].msch.bChannelReceiveFirstPackage[MCS_SERVER_CHANNEL_NUMBER_RETURN] = TRUE;
         return (CLCH_ERROR);
      } /* end "if (channel_receive_status == CLCH_RECEIVE_STATUS_ERROR)" */

   } /* end "for (;;)" */
return 0;
} /* end "clch_wait_for_receive_dynamic" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_channel_send_byte / ...word / ...long                 */
/*                                                                            */
/*   INPUT:        channel_number                                             */
/*                 p_data           pointer to data item                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function sends a data item (8, 16 or 32-bit) over     */
/*                 a logical channel.                                         */
/*                                                                            */
/******************************************************************************/

void clch_channel_send_byte (UNSIGNED16 channel_number, UNSIGNED8 data_byte)
{
    clch_channel_send_byte_idx(channel_number, data_byte, DG_DEFAULT_IDX);
}
void clch_channel_send_byte_idx (UNSIGNED16 channel_number, UNSIGNED8 data_byte, DG_U8 card_index)
{

   *(dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number]++) = data_byte;

} /* end "clch_channel_send_byte" */


void clch_channel_send_word (UNSIGNED16 channel_number, const UNSIGNED16 * p_data)
{
    clch_channel_send_word_idx(channel_number, p_data, DG_DEFAULT_IDX);
}
void clch_channel_send_word_idx (UNSIGNED16 channel_number, const UNSIGNED16 * p_data, DG_U8 card_index)
{
UNSIGNED8 * p_dest;
const UNSIGNED8 * p_source;

   p_dest = dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number];
   p_source = (const UNSIGNED8*)p_data;

   #if defined (O_LE)
      *(p_dest++) = p_source[1];
      *(p_dest++) = p_source[0];
   #else
      *(p_dest++) = *(p_source++);
      *(p_dest++) = *(p_source++);
   #endif

   dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number] = p_dest;

} /* end "clch_channel_send_word" */


void clch_channel_send_long (UNSIGNED16 channel_number, const UNSIGNED32 * p_data)
{
    clch_channel_send_long_idx(channel_number, p_data, DG_DEFAULT_IDX);
}
void clch_channel_send_long_idx (UNSIGNED16 channel_number, const UNSIGNED32 * p_data, DG_U8 card_index)
{
UNSIGNED8 * p_dest;
const UNSIGNED8 * p_source;

   p_dest = dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number];
   p_source = (const UNSIGNED8*)p_data;

   #if defined (O_LE)
      *(p_dest++) = p_source[3];
      *(p_dest++) = p_source[2];
      *(p_dest++) = p_source[1];
      *(p_dest++) = p_source[0];
   #else
      *(p_dest++) = *(p_source++);
      *(p_dest++) = *(p_source++);
      *(p_dest++) = *(p_source++);
      *(p_dest++) = *(p_source++);
   #endif

   dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number] = p_dest;

} /* end "clch_channel_send_long" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_channel_send_array_...                                */
/*                                                                            */
/*   INPUT:        channel_number                                             */
/*                 p_data           pointer to data items                     */
/*                 number_data      number of data items                      */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function sends an array of data over                  */
/*                 a logical channel.                                         */
/*                                                                            */
/******************************************************************************/

CLCH_RESULT clch_channel_send_array_byte (UNSIGNED16 channel_number, const UNSIGNED8 * p_data, UNSIGNED32 number_data)
{
    return clch_channel_send_array_byte_idx(channel_number, p_data, number_data, DG_DEFAULT_IDX);
}
CLCH_RESULT clch_channel_send_array_byte_idx (UNSIGNED16 channel_number, const UNSIGNED8 * p_data, UNSIGNED32 number_data, DG_U8 card_index)
{
/* NOTES:                                                   */
/* - the input parameter "number_data" is never 0           */
/* - this function is used to send dynamic data packages,   */
/*   so mutex for the channel must be taken again after     */
/*   call of the function "clch_channel_send_package"       */
/* - this function is optimized, since it is allways        */
/*   called before function "clch_channel_close"            */
UNSIGNED32  number_counter;
UNSIGNED32  number_send;
const UNSIGNED8 * p_source;
UNSIGNED8 * p_dest;
UNSIGNED32  counter;
BOOLEAN1    bFirstPackage;

   number_counter = number_data;
   p_source = (const UNSIGNED8*)p_data;
   bFirstPackage = TRUE;
   do
   {
      if (clch_channel_send_package_idx(channel_number, bFirstPackage, FALSE, card_index) != CLCH_OK)
      {
         return (CLCH_ERROR);
      } /* end "if (clch_channel_send_package(channel_number, bFirstPackage, FALSE) != CLCH_OK)" */

      bFirstPackage = FALSE;

      if (number_counter > MCS_CLIENT_CHANNEL_PACKAGE_SIZE_MAX)
      {
         number_send = MCS_CLIENT_CHANNEL_PACKAGE_SIZE_MAX;
         number_counter = number_counter - number_send;
      }
      else
      {
         number_send = number_counter;
         number_counter = 0;
      } /* end "if (number_counter > MCS_CLIENT_CHANNEL_PACKAGE_SIZE_MAX)" */

      p_dest = (UNSIGNED8*)p_channel_send_package_data[card_index][channel_number];
      counter = 0;
      do
      {
         *(p_dest++) = *(p_source++);
         counter++;
      } while (counter < number_send);

      dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number] = (UNSIGNED8*)p_dest;

   } while (number_counter > 0);

   return (CLCH_OK);

} /* end "clch_channel_send_array_byte" */


void clch_channel_send_array_word (UNSIGNED16 channel_number, const UNSIGNED16 * p_data, UNSIGNED16 number_data)
{
    clch_channel_send_array_word_idx(channel_number, p_data, number_data, DG_DEFAULT_IDX);
}
void clch_channel_send_array_word_idx (UNSIGNED16 channel_number, const UNSIGNED16 * p_data, UNSIGNED16 number_data, DG_U8 card_index)
{
/* NOTES:                                             */
/* - the input parameter "number_data" is never 0     */
/* - this function is used to send a process data     */
/*   port/mask value                                  */
/* - this function is optimized, since never an       */
/*   overflow of the package occurs                   */
/* - this function is optimized, since it is allways  */
/*   called before function "clch_channel_close"      */
UNSIGNED8 * p_dest;
const UNSIGNED8 * p_source;
UNSIGNED16  number_counter = 0;

   p_dest = (UNSIGNED8*)dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number];
   p_source = (const UNSIGNED8*)p_data;

   do
   {
      #if defined (O_LE)
         *(p_dest++) = p_source[1];
         *(p_dest++) = p_source[0];
         p_source += 2;
      #else
         *(p_dest++) = *(p_source++);
         *(p_dest++) = *(p_source++);
      #endif

      number_counter++;

   } while (number_counter < number_data);

   dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number] = (UNSIGNED8*)p_dest;

} /* end "clch_channel_send_array_word" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_channel_send_package                                  */
/*                                                                            */
/*   INPUT:        channel_number   MCS_CLIENT_CHANNEL_NUMBER_...             */
/*                 bFirstPackage    TRUE, FALSE                               */
/*                 bLastPackage     TRUE, FALSE                               */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: CLCH_RESULT      CLCH_OK, CLCH_ERROR                       */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function sends the current package over               */
/*                 a logical channel.                                         */
/*                                                                            */
/******************************************************************************/

CLCH_RESULT clch_channel_send_package (UNSIGNED16 channel_number, BOOLEAN1 bFirstPackage, BOOLEAN1 bLastPackage)
{
    return clch_channel_send_package_idx(channel_number, bFirstPackage, bLastPackage, DG_DEFAULT_IDX);
}
CLCH_RESULT clch_channel_send_package_idx (UNSIGNED16 channel_number, BOOLEAN1 bFirstPackage, BOOLEAN1 bLastPackage, DG_U8 card_index)
{
UNSIGNED8   *p_package;
UNSIGNED16  package_data_size;
UNSIGNED16  counter;
UNSIGNED8   package_crc;
UNSIGNED8   *p_package_pointer;
CPIL_RESULT cpil_result = CPIL_OK;
BOOLEAN1    bCallback = FALSE;

    p_package = p_channel_send_package_header[card_index][channel_number];
    package_data_size = (UNSIGNED16)(dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number] - p_package);

    /* NOTE:                                        */
    /* Notwendig für "lp_init", da die Funktion     */
    /* "clch_channel_send_word" mehrmals verwendet  */
    /* wird und ein Overflow des Package möglich    */
    /* ist.                                         */
    dg_card_config[card_index].msch.p_channel_send_package_pointer[channel_number] = \
        p_channel_send_package_data[card_index][channel_number];

    /* build package header */
    p_package[0] = (UNSIGNED8)MCS_CLIENT_CHANNEL_HEADER_START_DELIMITER;
    p_package[1] = (UNSIGNED8)channel_number;
    p_package[2] = (UNSIGNED8)(package_data_size - MCS_CLIENT_CHANNEL_HEADER_SIZE);
    p_package[3] = (UNSIGNED8)0x00; /* dummy CRC */

    if ((channel_number == MCS_CLIENT_CHANNEL_NUMBER_MESSAGE_DATA) && \
        (bFirstPackage == TRUE) && (bLastPackage == TRUE))
    {
        if ((p_package[4] == MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_INFO) || \
            (p_package[4] == MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_NEXT) || \
            (p_package[4] == MCS_FUNCTION_NUMBER_GF_GET_CALLBACK_PACKAGES))
        {
            p_package[1] = (UNSIGNED8)MCS_CLIENT_CHANNEL_NUMBER_CONFIG;
            bCallback = TRUE;
        } /* if ((...)) */
    } /* if ((bFirstPackage == TRUE) && (bLastPackage == TRUE)) */

    /* calculate "package_crc" */
    package_crc = 0;
    p_package_pointer = p_package;
    counter = 0;
    do
    {
        package_crc = (UNSIGNED8)(package_crc + *(p_package_pointer++));
        counter++;
    } while (counter < package_data_size);
    /* put "package_crc" into package header */
    p_package[3] = (UNSIGNED8)(~package_crc + 1);

#if defined (O_PRINT_DEBUG_CH)
    printf("|H%d%dA>", channel_number, bLastPackage);
#endif

#if !defined (O_OS_SINGLE)
    if (bCallback)
    {
        cpil_mutex_take (MCS_CLIENT_CHANNEL_NUMBER_CONFIG);
    } /* if (bCallback) */
    cpil_mutex_take (CPIL_MUTEX_HARDWARE_CHANNEL);
#else
    (void)bCallback;
#endif

#if defined (O_PRINT_DEBUG_CH)
    printf("|H%d%dB>", channel_number, bLastPackage);
#endif

#if !defined (O_OS_SINGLE)
    if (dg_card_config[card_index].mscf.cgf_server_status != CGF_SERVER_STATUS_RUN)
    {
        if ((dg_card_config[card_index].mscf.cgf_server_status == CGF_SERVER_STATUS_STOP)                               &&
            (p_package[0]      == (UNSIGNED8)MCS_CLIENT_CHANNEL_HEADER_START_DELIMITER) &&
            (p_package[1]      == (UNSIGNED8)MCS_CLIENT_CHANNEL_NUMBER_CONFIG)          &&
            (p_package[2]      == (UNSIGNED8)0x02) /* package size */                   &&
            (p_package[3]      == (UNSIGNED8)0x9E) /* package CRC  */                   &&
            (p_package[4]      == (UNSIGNED8)MCS_FUNCTION_NUMBER_GF_RESTART_DEVICE)     &&
            (p_package[5]      == (UNSIGNED8)CLCH_INSTANCE_NUMBER_DEFAULT))
        {
            /* accept */
            ;
        }
        else
        {
#if !defined (O_OS_SINGLE)
            if (bCallback)
            {
                cpil_mutex_give (MCS_CLIENT_CHANNEL_NUMBER_CONFIG);
            } /* if (bCallback) */
#endif
            /* discard */
            return (CLCH_ERROR);
        } /* end "if ((...))" */
    } /* if (cgf_server_status != CGF_SERVER_STATUS_RUN) */
#endif /* !O_OS_SINGLE */

#ifdef O_DEBUG_FCT_NR
if ((bFirstPackage == FALSE) && (bLastPackage == FALSE))
{
    fprintf(stdout, "a%d,", p_package[4]); fflush(stdout);
}
else if ((bFirstPackage == TRUE) && (bLastPackage == FALSE))
{
    fprintf(stdout, "b%d,", p_package[4]); fflush(stdout);
}
else if ((bFirstPackage == FALSE) && (bLastPackage == TRUE))
{
    fprintf(stdout, "c%d,", p_package[4]); fflush(stdout);
}
else if ((bFirstPackage == TRUE) && (bLastPackage == TRUE))
{
    fprintf(stdout, "d%d,", p_package[4]); fflush(stdout);
}
#endif /* O_DEBUG_FCT_NR */

#ifdef O_DEBUG_FCT_NR
if (bLastPackage == FALSE)
{
    fprintf(stdout, "a%d,", p_package[4]); fflush(stdout);
}
else
{
    if (((p_package[4] >= 20) && (p_package[4] < 30)) || ((p_package[4] >= 120) && (p_package[4] < 170)))
    {
        fprintf(stdout, "b%d,", p_package[4]); fflush(stdout);
    }
}
#endif /* O_DEBUG_FCT_NR */

    cpil_result = cpil_hardware_send_idx (p_package, package_data_size, card_index);

#if !defined (O_OS_SINGLE)
    if (bCallback)
    {
        cpil_mutex_give (MCS_CLIENT_CHANNEL_NUMBER_CONFIG);
    } /* if (bCallback) */
#endif

    if (cpil_result != CPIL_OK)
    {
        return (CLCH_ERROR);
    } /* end "if (cpil_result != CPIL_OK)" */

#if !defined (O_OS_SINGLE)
    if (bLastPackage == FALSE)
    {
        #if defined (O_PRINT_DEBUG_CH)
            printf("<H%d%d|", channel_number, bLastPackage);
        #endif
        cpil_mutex_give (CPIL_MUTEX_HARDWARE_CHANNEL);
        cpil_idle_wait ();
    } /* if (bLastPackage == FALSE) */
#endif /* !O_OS_SINGLE */

    return (CLCH_OK);

} /* end "clch_channel_send_package" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_receive_byte, ..._word, ..._port_value                */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function receives data from the logical channels.     */
/*                                                                            */
/******************************************************************************/

void clch_receive_byte (UNSIGNED8 * p_data)
{
    clch_receive_byte_idx(p_data, DG_DEFAULT_IDX);
}
void clch_receive_byte_idx (UNSIGNED8 * p_data, DG_U8 card_index)
{
#if defined (O_RECEIVE_CRC)
    cpil_hardware_receive_idx (p_data, card_index);
    /* calculate CRC */
    dg_card_config[card_index].msch.channel_receive_crc += *p_data;
#else
    cpil_hardware_receive_idx (p_data, card_index);
#endif
} /* end "clch_receive_byte" */


void clch_receive_word (UNSIGNED16 * p_data)
{
    clch_receive_word_idx(p_data, DG_DEFAULT_IDX);
}
void clch_receive_word_idx (UNSIGNED16 * p_data, DG_U8 card_index)
{
UNSIGNED8 *    p_pointer;
#if defined (O_RECEIVE_CRC)
   UNSIGNED8   crc;
#endif
   p_pointer = (UNSIGNED8*)p_data;

   #if defined (O_LE)
      #if defined (O_RECEIVE_CRC)
         cpil_hardware_receive_idx (&p_pointer[1], card_index);
         crc = p_pointer[1];
         cpil_hardware_receive_idx (&p_pointer[0], card_index);
         crc += p_pointer[0];
      #else
         cpil_hardware_receive_idx (&p_pointer[1], card_index);
         cpil_hardware_receive_idx (&p_pointer[0], card_index);
      #endif
   #else
      #if defined (O_RECEIVE_CRC)
         cpil_hardware_receive_idx (p_pointer, card_index);
         crc = *(p_pointer++);
         cpil_hardware_receive_idx (p_pointer, card_index);
         crc += *(p_pointer++);
      #else
         cpil_hardware_receive_idx (p_pointer++, card_index);
         cpil_hardware_receive_idx (p_pointer++, card_index);
      #endif
   #endif

   #if defined (O_RECEIVE_CRC)
      /* calculate CRC */
      dg_card_config[card_index].msch.channel_receive_crc += crc;
   #endif

} /* end "clch_receive_word" */


void clch_receive_long (UNSIGNED32 * p_data)
{
    clch_receive_long_idx(p_data, DG_DEFAULT_IDX);
}
void clch_receive_long_idx (UNSIGNED32 * p_data, DG_U8 card_index)
{
UNSIGNED8 *    p_pointer;
#if defined (O_RECEIVE_CRC)
   UNSIGNED8   crc;
#endif
   p_pointer = (UNSIGNED8*)p_data;

   #if defined (O_LE)
      #if defined (O_RECEIVE_CRC)
         cpil_hardware_receive_idx (&p_pointer[3], card_index);
         crc = p_pointer[3];
         cpil_hardware_receive_idx (&p_pointer[2], card_index);
         crc += p_pointer[2];
         cpil_hardware_receive_idx (&p_pointer[1], card_index);
         crc += p_pointer[1];
         cpil_hardware_receive_idx (&p_pointer[0], card_index);
         crc += p_pointer[0];
      #else
         cpil_hardware_receive_idx (&p_pointer[3], card_index);
         cpil_hardware_receive_idx (&p_pointer[2], card_index);
         cpil_hardware_receive_idx (&p_pointer[1], card_index);
         cpil_hardware_receive_idx (&p_pointer[0], card_index);
      #endif
   #else
      #if defined (O_RECEIVE_CRC)
         cpil_hardware_receive_idx (p_pointer, card_index);
         crc = *(p_pointer++);
         cpil_hardware_receive_idx (p_pointer, card_index);
         crc += *(p_pointer++);
         cpil_hardware_receive_idx (p_pointer, card_index);
         crc += *(p_pointer++);
         cpil_hardware_receive_idx (p_pointer, card_index);
         crc += *(p_pointer++);
      #else
         cpil_hardware_receive_idx (p_pointer++, card_index);
         cpil_hardware_receive_idx (p_pointer++, card_index);
         cpil_hardware_receive_idx (p_pointer++, card_index);
         cpil_hardware_receive_idx (p_pointer++, card_index);
      #endif
   #endif

   #if defined (O_RECEIVE_CRC)
      /* calculate CRC */
      dg_card_config[card_index].msch.channel_receive_crc += crc;
   #endif

} /* end "clch_receive_long" */


void clch_receive_array_byte (UNSIGNED8 * p_data, UNSIGNED32 number_data)
{
    clch_receive_array_byte_idx(p_data, number_data, DG_DEFAULT_IDX);
}
void clch_receive_array_byte_idx (UNSIGNED8 * p_data, UNSIGNED32 number_data, DG_U8 card_index)
{
UNSIGNED8 *    p_pointer;
UNSIGNED32     number_counter = 0;
#if defined (O_RECEIVE_CRC)
   UNSIGNED8   crc = 0;
#endif
   p_pointer = (UNSIGNED8*)p_data;
   do
   {
      #if defined (O_RECEIVE_CRC)
         cpil_hardware_receive_idx (p_pointer, card_index);
         crc += *(p_pointer++);
      #else
         cpil_hardware_receive_idx (p_pointer++, card_index);
      #endif

      number_counter++;

   } while (number_counter < number_data);

   #if defined (O_RECEIVE_CRC)
      /* calculate CRC */
      dg_card_config[card_index].msch.channel_receive_crc += crc;
   #endif

} /* end "clch_receive_array_byte" */


void clch_receive_array_word (UNSIGNED16 * p_data, UNSIGNED32 number_data)
{
    clch_receive_array_word_idx(p_data, number_data, DG_DEFAULT_IDX);
}
void clch_receive_array_word_idx (UNSIGNED16 * p_data, UNSIGNED32 number_data, DG_U8 card_index)
{
UNSIGNED8 *    p_pointer;
UNSIGNED32     number_counter = 0;
#if defined (O_RECEIVE_CRC)
   UNSIGNED8   crc = 0;
#endif
   p_pointer = (UNSIGNED8*)p_data;

   do
   {
      #if defined (O_LE)
         #if defined (O_RECEIVE_CRC)
            cpil_hardware_receive_idx (&p_pointer[1], card_index);
            crc += p_pointer[1];
            cpil_hardware_receive_idx (&p_pointer[0], card_index);
            crc += p_pointer[0];
            p_pointer += 2;
         #else
            cpil_hardware_receive_idx (&p_pointer[1], card_index);
            cpil_hardware_receive_idx (&p_pointer[0], card_index);
            p_pointer += 2;
         #endif
      #else
         #if defined (O_RECEIVE_CRC)
            cpil_hardware_receive_idx (p_pointer, card_index);
            crc += *(p_pointer++);
            cpil_hardware_receive_idx (p_pointer, card_index);
            crc += *(p_pointer++);
         #else
            cpil_hardware_receive_idx (p_pointer++, card_index);
            cpil_hardware_receive_idx (p_pointer++, card_index);
         #endif
      #endif

      number_counter++;

   } while (number_counter < number_data);

   #if defined (O_RECEIVE_CRC)
      /* calculate CRC */
      dg_card_config[card_index].msch.channel_receive_crc += crc;
   #endif

} /* end "clch_receive_array_word" */


#if defined (O_RECEIVE_CRC)
/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     clch_receive_check_crc                                     */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function checks the CRC of the received data bytes.   */
/*                                                                            */
/******************************************************************************/

CLCH_RESULT clch_receive_check_crc (void)
{
    return clch_receive_check_crc_idx(DG_DEFAULT_IDX);
}
CLCH_RESULT clch_receive_check_crc_idx (DG_U8 card_index)
{

   if (dg_card_config[card_index].msch.channel_receive_crc != 0)
   {
      cgf_put_device_status (GF_DEVICE_STATUS_CLIENT_RECEIVE_CRC, card_index);
   } /* end "if (channel_receive_crc != 0)" */
   return (dg_card_config[card_index].msch.channel_receive_crc);

} /* end "clch_receive_check_crc" */
#endif /* end "#if defined (O_RECEIVE_CRC)" */


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif
