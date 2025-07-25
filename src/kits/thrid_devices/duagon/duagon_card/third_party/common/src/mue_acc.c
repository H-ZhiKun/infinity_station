/* ==========================================================================
 *
 *  File      : MUE_ACC.C
 *
 *  Purpose   : MVB UART Emulation - Access Interface
 *              - UART: MVB UART Emulation 'MDFULL'
 *
 *  Project   : TCN Driver Software for D013
 *              - MVB UART Emulation Protocol (d-000206-nnnnnn)
 *              - TCN Software Architecture   (d-000487-nnnnnn)
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
 *  Pre-processor Definitions:
 *  --------------------------
 *  - MUE_ACC_HW_LE   - the MVB UART Emulation of the MVB interface board
 *                      is configured to handle words (16-bit values) with
 *                      little-endian number representation; this applies
 *                      to parameters and data of all MVB UART Emulation
 *                      commands
 *                      (default is big-endian number representation)
 *  - MUE_ACC_PRINT   - debug printouts
 *
 * ==========================================================================
 */


/* ==========================================================================
 *
 *  Project specific Definitions used for Conditional Compiling
 *
 * ==========================================================================
 */
#ifdef TCN_PRJ
#   include <tcn_prj.h>
#endif


/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  TCN Software Architecture
 * --------------------------------------------------------------------------
 */
#include <tcn_def.h>

/* --------------------------------------------------------------------------
 *  MVB UART Emulation
 * --------------------------------------------------------------------------
 */
#include <mue_def.h>

/* --------------------------------------------------------------------------
 *  Communications channel
 * --------------------------------------------------------------------------
 */
#include "dg_hdio.h"
#include "dg_hdc.h"


#include "mue_acc.h"
/* ==========================================================================
 *
 *  Definitions
 *
 * ==========================================================================
 */
#if defined (MUE_ACC_HW_LE) && defined (TCN_LE)
#   undef  MUE_ACC_BYTE_SWAP
#endif
#if defined (MUE_ACC_HW_LE) && !defined (TCN_LE)
#   define MUE_ACC_BYTE_SWAP
#endif
#if !defined (MUE_ACC_HW_LE) && defined (TCN_LE)
#   define MUE_ACC_BYTE_SWAP
#endif
#if !defined (MUE_ACC_HW_LE) && !defined (TCN_LE)
#   undef  MUE_ACC_BYTE_SWAP
#endif


/* ==========================================================================
 *
 *  Local Variables - Communication Channel (common)
 *
 * ==========================================================================
 */

/* ==========================================================================
 *
 *  Public Procedures (general)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_timeout_calibrate
 *
 *  Purpose   : Calibrate the timeout value for getting ready status bits
 *              of the UART register LSR.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_timeout_calibrate
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED32  timeout_cal
 *              );
 *
 *  Input     : p_bus_ctrl  - pointer to bus controller specific values
 *              timeout_cal - timeout value used for calibration
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 *              - The structure of this procedure is the same like
 *                "mue_acc_tx_start()" and "mue_acc_rx_wait()".
 *              - For calibration of the timeout value the condition for
 *                UART register LSR will never hit.
 *                This will trigger the timeout.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_timeout_calibrate
(
    void        *p_bus_ctrl,
    UNSIGNED32  timeout_cal
)
{
    /* avoid warnings */
    (void) p_bus_ctrl;
    (void) timeout_cal;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_timeout_calibrate()\n");
#endif /* #ifdef MUE_ACC_PRINT */

    return(MUE_RESULT_TIMEOUT);

} /* mue_acc_timeout_calibrate */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_init
 *
 *  Purpose   : Initialises the access to the UART (check for availability).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_init
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_init
(
    void        *p_bus_ctrl
)
{
    return mue_acc_init_idx(p_bus_ctrl, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_init_idx
(
    void        *p_bus_ctrl,
    DG_U8        card_index
)
{
    MUE_RESULT      mue_result;
    DG_RESULT  hdio_result;

    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_init()\n");
#endif /* #ifdef MUE_ACC_PRINT */

    dg_card_config[card_index].muac.cch_data_ptr = dg_card_config[card_index].muac.cch_data_buf;
    hdio_result = hdc_init(card_index);
    if ( DG_OK==hdio_result ) {
        mue_result = MUE_RESULT_OK;
    } else if ( DG_ERROR_TIMEOUT==hdio_result ) {
        mue_result = MUE_RESULT_TIMEOUT;
    } else {
        mue_result = MUE_RESULT_ERROR;
    }
    return(mue_result);

} /* mue_acc_init */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_clean_up
 *
 *  Purpose   : Perform a clean-up of the UART communication
 *              (synchronise access protocol of the MVB UART Emulation).
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_clean_up
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - If a process using the MVB UART Emulation was interrupted
 *                the access protocol of the MVB UART Emulation remains in
 *                one of the following three states (Sx):
 *                S1: The MVB UART Emulation is ready to receive a new
 *                    command character -> nothing must be done
 *                    * any transmitted data bytes with value 0x00 (see
 *                      state S2) will be ignored
 *                S2: A MVB UART Emulation command is still in progress and
 *                    requests more data bytes (in minimum the command
 *                    character was sent) -> synchronisation is necessary
 *                    * transmits 34 data bytes with value 0x00 (command 'P'
 *                      requests the max. number of data bytes; PD_16=33,
 *                      PD_16F=33, PD_FULL=34, i.e. 34 data bytes); after
 *                      this the MVB UART Emulation may enter the state S3
 *                S3: A MVB UART Emulation command is still in progress and
 *                    transmits more data bytes -> synchronisation is
 *                    necessary
 *                    * transmit any command character which flushes the
 *                      transmit queue of the MVB UART Emulation
 *                    or (alternative)
 *                    * receive until nothing left (command 'G' transmits
 *                      the max. number of data bytes; PD_16=33, PD_16F=35,
 *                      PD_FULL=35, i.e. 35 data bytes)
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_clean_up
(
    void        *p_bus_ctrl
)
{
    return mue_acc_clean_up_idx(p_bus_ctrl, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_clean_up_idx
(
    void        *p_bus_ctrl,
    DG_U8        card_index
)
{
    MUE_RESULT mue_result=MUE_RESULT_OK;
#if DG_IO_MODE != DG_HDCFG_PARALLEL
    DG_RESULT  hdio_result;
    UNSIGNED16 c;
    DG_S32 function_time_out;
    DG_S32 function_start_time;

    function_start_time = osl_get_us_time_stamp();
    function_time_out=0;
#endif

             
    /* avoid warnings */
    (void) p_bus_ctrl;
    #ifdef MUE_ACC_PRINT
        MUE_OSL_PRINTF("mue_acc_clean_up()\n");
    #endif /* #ifdef MUE_ACC_PRINT */
    
    if (card_index == DG_NO_CARD) {
        return MUE_RESULT_PARAMETER; /* wrong parameter, card index for MVB opps.not set */
    }
    if ( !(DG_HDIO_IS_INITIALIZED == dg_card_config[card_index].is_initialized) ||
         NO_MVB_CHANNEL == CARD_MVB_CHANNEL_NUM(card_index) )
    {
#ifdef MUE_ACC_PRINT    
        MUE_OSL_PRINTF("ERROR: card-index %d not initialised ()\n",card_index);
#endif        
        mue_result = MUE_RESULT_ERROR;
    }
#if DG_IO_MODE != DG_HDCFG_PARALLEL
    if ( MUE_RESULT_OK == mue_result ) {
        function_time_out = dg_card_config[card_index].dg_hdio_configs[CARD_MVB_CHANNEL_NUM(card_index)].function_timeout;

        /* ----------------------------------------------------------------------
         *  transmit 34 bytes with value 0x00
         * ----------------------------------------------------------------------
         */
        dg_card_config[card_index].muac.cch_data_cnt = 0;
        dg_card_config[card_index].muac.cch_data_ptr = dg_card_config[card_index].muac.cch_data_buf;

        for (c=0; c<34; c++)
        {
            dg_card_config[card_index].muac.cch_data_cnt++;
            *dg_card_config[card_index].muac.cch_data_ptr++ = 0x00;
        } /* for (c=0; c<34; c++) */

        hdio_result = dg_hdio_transmit(CARD_MVB_CHANNEL_NUM(card_index),dg_card_config[card_index].muac.cch_data_cnt, dg_card_config[card_index].muac.cch_data_buf, function_time_out, function_start_time, card_index);
        if ( DG_OK==hdio_result ) {
            mue_result = MUE_RESULT_OK;
        } else if ( DG_ERROR_TIMEOUT==hdio_result ) {
            mue_result = MUE_RESULT_TIMEOUT;
        } else {
            mue_result = MUE_RESULT_ERROR;
        }
    }
    /* ----------------------------------------------------------------------
     *  receive max. 35 bytes
     * ----------------------------------------------------------------------
     */
    if (MUE_RESULT_OK == mue_result)
    {
        dg_card_config[card_index].muac.cch_data_cnt = 0;
        dg_card_config[card_index].muac.cch_data_ptr = dg_card_config[card_index].muac.cch_data_buf;

        hdio_result = dg_hdio_receive(CARD_MVB_CHANNEL_NUM(card_index),35, dg_card_config[card_index].muac.cch_data_buf, card_index);
        if ( DG_OK==hdio_result || DG_ERROR_TIMEOUT==hdio_result ) {
            /* ignore MUE_RESULT_TIMEOUT */
            mue_result = MUE_RESULT_OK;
        } else {
            mue_result = MUE_RESULT_ERROR;
        }

    } /* if (MUE_RESULT_OK == mue_result) */
#endif
    return(mue_result);

} /* mue_acc_clean_up */


/* ==========================================================================
 *
 *  Public Procedures for Transmit (TX)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_start
 *
 *  Purpose   : Starts a transmission over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_tx_start
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of bytes to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_start
(
    void        *p_bus_ctrl,
    UNSIGNED16  size
)
{
    return mue_acc_tx_start_idx(p_bus_ctrl, size, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_start_idx
(
    void        *p_bus_ctrl,
    UNSIGNED16   size,
    DG_U8        card_index
)
{

    /* avoid warnings */
    (void) p_bus_ctrl;
    (void) size;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_tx_start(size=%d)\n", size);
#endif /* #ifdef MUE_ACC_PRINT */

    if (card_index == DG_NO_CARD) {
        return MUE_RESULT_PARAMETER; /* wrong parameter, card index for MVB opps.not set */
    }
    if ( DG_OK==dg_hdio_wait_to_send(CARD_MVB_CHANNEL_NUM(card_index)) ) {
        dg_card_config[card_index].muac.cch_data_cnt = 0;
        dg_card_config[card_index].muac.cch_data_ptr = dg_card_config[card_index].muac.cch_data_buf;
    
        return (MUE_RESULT_OK);

    } else {
        return (MUE_RESULT_TIMEOUT);
    }

} /* mue_acc_tx_start */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_value8
 *
 *  Purpose   : Transmits a 8-bit value over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_tx_value8
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD8       value8
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              value8     - 8-bit value to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for general parameters.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_value8
(
    void        *p_bus_ctrl,
    WORD8        value8
)
{
    return mue_acc_tx_value8_idx(p_bus_ctrl, value8, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_value8_idx
(
    void        *p_bus_ctrl,
    WORD8        value8,
    DG_U8        card_index
)
{


    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_tx_value8(0x%02X)\n", value8);
#endif /* #ifdef MUE_ACC_PRINT */

    dg_card_config[card_index].muac.cch_data_cnt++;
    *dg_card_config[card_index].muac.cch_data_ptr++ = value8;

    return(MUE_RESULT_OK);

} /* mue_acc_tx_value8 */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_value16
 *
 *  Purpose   : Transmits a 16-bit value over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_tx_value16
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      value16
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              value16    - 16-bit value to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for general parameters.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_value16
(
    void        *p_bus_ctrl,
    WORD16       value16
)
{
    return mue_acc_tx_value16_idx(p_bus_ctrl, value16, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_value16_idx
(
    void        *p_bus_ctrl,
    WORD16       value16,
    DG_U8        card_index
)
{
    WORD8       *p_word8 = (WORD8*)&value16;


    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_tx_value16(0x%04X)\n", value16);
#endif /* #ifdef MUE_ACC_PRINT */

#ifdef MUE_ACC_BYTE_SWAP

    /* first low then high byte */
    dg_card_config[card_index].muac.cch_data_cnt++;
    *dg_card_config[card_index].muac.cch_data_ptr++ = p_word8[1];
    dg_card_config[card_index].muac.cch_data_cnt++;
    *dg_card_config[card_index].muac.cch_data_ptr++ = p_word8[0];

#else /* #ifdef MUE_ACC_BYTE_SWAP */

    /* first high then low byte */
    dg_card_config[card_index].muac.cch_data_cnt++;
    *dg_card_config[card_index].muac.cch_data_ptr++ = *p_word8++;
    dg_card_config[card_index].muac.cch_data_cnt++;
    *dg_card_config[card_index].muac.cch_data_ptr++ = *p_word8;

#endif /* #else */

    return(MUE_RESULT_OK);

} /* mue_acc_tx_value16 */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_array
 *
 *  Purpose   : Transmits data of a buffer over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_tx_array
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size,
 *                  void        *p_buffer
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - size of the buffer (number of bytes to
 *                           transmit over the UART)
 *              p_buffer   - pointer to buffer that contains data
 *                           to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for data related to a MVB process data port
 *                (size = 2, 4, 8, 16 or 32 bytes).
 *              - Used for data related to a MVB message data frame
 *                (size = 32 bytes).
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_array
(
    void        *p_bus_ctrl,
    UNSIGNED16   size,
    void        *p_buffer
)
{
    return mue_acc_tx_array_idx(p_bus_ctrl, size, p_buffer, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_array_idx
(
    void        *p_bus_ctrl,
    UNSIGNED16   size,
    void        *p_buffer,
    DG_U8        card_index
)
{
    UNSIGNED16  i;
    WORD8       *p_word8 = (WORD8*)p_buffer;


    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_tx_array(size=%d)\n", size);
#endif /* #ifdef MUE_ACC_PRINT */

#ifdef MUE_ACC_HW_LE

    for (i=0; i<size; i+=2)
    {
        dg_card_config[card_index].muac.cch_data_cnt++;
        *dg_card_config[card_index].muac.cch_data_ptr++ = p_word8[i+1]; /* low  byte */
        dg_card_config[card_index].muac.cch_data_cnt++;
        *dg_card_config[card_index].muac.cch_data_ptr++ = p_word8[i+0]; /* high byte */
    } /* for (i=0; i<size; i+=2) */

#else /* #ifdef MUE_ACC_HW_LE */

    for (i=0; i<size; i+=2)
    {
        dg_card_config[card_index].muac.cch_data_cnt++;
        *dg_card_config[card_index].muac.cch_data_ptr++ = *p_word8++; /* high byte */
        dg_card_config[card_index].muac.cch_data_cnt++;
        *dg_card_config[card_index].muac.cch_data_ptr++ = *p_word8++; /* low  byte */
    } /* for (i=0; i<size; i+=2) */

#endif /* #else */

    return(MUE_RESULT_OK);

} /* mue_acc_tx_array */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_fill
 *
 *  Purpose   : Transmits fill bytes over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_tx_fill
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of fill bytes to transmit over the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for fill bytes related to a MVB process data port
 *                (size = 0, 16, 24, 28 or 30 bytes).
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_fill
(
    void        *p_bus_ctrl,
    UNSIGNED16   size
)
{
    return mue_acc_tx_fill_idx(p_bus_ctrl, size, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_fill_idx
(
    void        *p_bus_ctrl,
    UNSIGNED16   size,
    DG_U8        card_index
)
{
    UNSIGNED16  i;


    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_tx_fill(size=%d)\n", size);
#endif /* #ifdef MUE_ACC_PRINT */

    for (i=0; i<size; i++)
    {
        dg_card_config[card_index].muac.cch_data_cnt++;
        *dg_card_config[card_index].muac.cch_data_ptr++ = 0x00;
    } /* for (i=0; i<size; i++) */

    return(MUE_RESULT_OK);

} /* mue_acc_tx_fill */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_tx_trigger
 *
 *  Purpose   : Triggers a transmission over the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_tx_trigger
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_trigger
(
    void        *p_bus_ctrl
)
{
    return mue_acc_tx_trigger_idx(p_bus_ctrl, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_tx_trigger_idx
(
    void        *p_bus_ctrl,
    DG_U8        card_index
)
{
    MUE_RESULT mue_result=MUE_RESULT_OK;
    DG_RESULT  hdio_result=!DG_OK;

    DG_S32 function_time_out=0;
    DG_S32 function_start_time = osl_get_us_time_stamp();

    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_tx_trigger()\n");
#endif /* #ifdef MUE_ACC_PRINT */
    if (card_index == DG_NO_CARD) {
        return MUE_RESULT_PARAMETER; /* wrong parameter, card index for MVB opps.not set */
    }
    if ( !(DG_HDIO_IS_INITIALIZED == dg_card_config[card_index].is_initialized) ||
         NO_MVB_CHANNEL == CARD_MVB_CHANNEL_NUM(card_index) )
    {
#ifdef MUE_ACC_PRINT    
        MUE_OSL_PRINTF("ERROR: card-index %d not initialised ()\n",card_index);
#endif        
        mue_result = MUE_RESULT_ERROR;
    }
    if ( MUE_RESULT_OK == mue_result ) {
        function_time_out = dg_card_config[card_index].dg_hdio_configs[CARD_MVB_CHANNEL_NUM(card_index)].function_timeout;
        hdio_result = dg_hdio_transmit(CARD_MVB_CHANNEL_NUM(card_index),dg_card_config[card_index].muac.cch_data_cnt, dg_card_config[card_index].muac.cch_data_buf, function_time_out, function_start_time, card_index);
    }
    if ( DG_OK==hdio_result ) {
        mue_result = MUE_RESULT_OK;
    } else if ( DG_ERROR_TIMEOUT==hdio_result ) {
        mue_result = MUE_RESULT_TIMEOUT;
    } else {
        mue_result = MUE_RESULT_ERROR;
    }
    
    return(mue_result);

} /* mue_acc_tx_trigger */


/* ==========================================================================
 *
 *  Public Procedures for Receive (RX)
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_wait
 *
 *  Purpose   : Waits, until something to receive from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_rx_wait
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of bytes to receive from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_wait
(
    void        *p_bus_ctrl,
    UNSIGNED16   size
)
{
    return mue_acc_rx_wait_idx(p_bus_ctrl, size, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_wait_idx
(
    void        *p_bus_ctrl,
    UNSIGNED16   size,
    DG_U8        card_index
)
{
    MUE_RESULT      mue_result;
    DG_RESULT  hdio_result;


    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_wait(size=%d)\n", size);
#endif /* #ifdef MUE_ACC_PRINT */

    if (card_index == DG_NO_CARD) {
        return MUE_RESULT_PARAMETER; /* wrong parameter, card index for MVB opps.not set */
    }
    dg_card_config[card_index].muac.cch_data_cnt = 0;
    dg_card_config[card_index].muac.cch_data_ptr = dg_card_config[card_index].muac.cch_data_buf;

    hdio_result = dg_hdio_receive(CARD_MVB_CHANNEL_NUM(card_index),size, dg_card_config[card_index].muac.cch_data_buf, card_index);
    if ( DG_OK==hdio_result ) {
        mue_result = MUE_RESULT_OK;
    } else if ( DG_ERROR_TIMEOUT==hdio_result ) {
        mue_result = MUE_RESULT_TIMEOUT;
    } else {
        mue_result = MUE_RESULT_ERROR;
    }
    
    if (MUE_RESULT_OK == mue_result)
    {
        dg_card_config[card_index].muac.cch_data_cnt = size;
    } /* if (MUE_RESULT_OK == mue_result) */

    return(mue_result);

} /* mue_acc_rx_wait */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_value8
 *
 *  Purpose   : Receives a 8-bit value from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_rx_value8
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD8       *p_value8
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Output    : p_value8   - pointer to buffer that receives a 8-bit value
 *                           from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for general parameters.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_value8
(
    void        *p_bus_ctrl,
    WORD8       *p_value8
)
{
    return mue_acc_rx_value8_idx(p_bus_ctrl, p_value8, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_value8_idx
(
    void        *p_bus_ctrl,
    WORD8       *p_value8,
    DG_U8        card_index
)
{


    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_value8()\n");
#endif /* #ifdef MUE_ACC_PRINT */

    dg_card_config[card_index].muac.cch_data_cnt++;

    *p_value8 = *dg_card_config[card_index].muac.cch_data_ptr++;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_value8(0x%02X)\n", *p_value8);
#endif /* #ifdef MUE_ACC_PRINT */

    return(MUE_RESULT_OK);

} /* mue_acc_tx_value8 */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_value16
 *
 *  Purpose   : Receives a 16-bit value from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_rx_value16
 *              (
 *                  void        *p_bus_ctrl,
 *                  WORD16      *p_value16
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Output    : p_value16  - pointer to buffer that receives a 16-bit value
 *                             from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for general parameters.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_value16
(
    void        *p_bus_ctrl,
    WORD16      *p_value16
)
{
    return mue_acc_rx_value16_idx(p_bus_ctrl, p_value16, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_value16_idx
(
    void        *p_bus_ctrl,
    WORD16      *p_value16,
    DG_U8        card_index
)
{
    WORD8       *p_word8 = (WORD8*)p_value16;


    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_value16()\n");
#endif /* #ifdef MUE_ACC_PRINT */

#ifdef MUE_ACC_BYTE_SWAP

    /* first low then high byte */
    dg_card_config[card_index].muac.cch_data_cnt++;
    p_word8[1] = *dg_card_config[card_index].muac.cch_data_ptr++;
    dg_card_config[card_index].muac.cch_data_cnt++;
    p_word8[0] = *dg_card_config[card_index].muac.cch_data_ptr++;

#else /* #ifdef MUE_ACC_BYTE_SWAP */

    /* first high then low byte */
    dg_card_config[card_index].muac.cch_data_cnt++;
    *p_word8++ = *dg_card_config[card_index].muac.cch_data_ptr++;
    dg_card_config[card_index].muac.cch_data_cnt++;
    *p_word8   = *dg_card_config[card_index].muac.cch_data_ptr++;

#endif /* #else */

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_value16(0x%04X)\n", *p_value16);
#endif /* #ifdef MUE_ACC_PRINT */

    return(MUE_RESULT_OK);

} /* mue_acc_rx_value16 */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_array
 *
 *  Purpose   : Receives data into a buffer from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_rx_array
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size,
 *                  void        *p_buffer
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - size of the buffer (number of bytes to receive
 *                           from the UART)
 *
 *  Output    : p_buffer   - pointer to buffer that receives data from
 *                           the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for data related to a MVB process data port
 *                (size = 2, 4, 8, 16 or 32 bytes).
 *              - Used for data related to a MVB message data frame
 *                (size = 32 bytes).
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_array
(
    void        *p_bus_ctrl,
    UNSIGNED16   size,
    void        *p_buffer
)
{
    return mue_acc_rx_array_idx(p_bus_ctrl, size, p_buffer, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_array_idx
(
    void        *p_bus_ctrl,
    UNSIGNED16  size,
    void        *p_buffer,
    DG_U8        card_index
)
{
    UNSIGNED16  i;
    WORD8       *p_word8 = (WORD8*)p_buffer;


    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_array(size=%d)\n", size);
#endif /* #ifdef MUE_ACC_PRINT */

#ifdef MUE_ACC_HW_LE

    for (i=0; i<size; i+=2)
    {
        dg_card_config[card_index].muac.cch_data_cnt++;
        p_word8[i+1] = *dg_card_config[card_index].muac.cch_data_ptr++; /* low  byte */
        dg_card_config[card_index].muac.cch_data_cnt++;
        p_word8[i+0] = *dg_card_config[card_index].muac.cch_data_ptr++; /* high byte */
    } /* for (i=0; i<size; i+=2) */

#else /* #ifdef MUE_ACC_HW_LE */

    for (i=0; i<size; i+=2)
    {
        dg_card_config[card_index].muac.cch_data_cnt++;
        *p_word8++ = *dg_card_config[card_index].muac.cch_data_ptr++; /* high byte */
        dg_card_config[card_index].muac.cch_data_cnt++;
        *p_word8++ = *dg_card_config[card_index].muac.cch_data_ptr++; /* low  byte */
    } /* for (i=0; i<size; i+=2) */

#endif /* #else */

    return(MUE_RESULT_OK);

} /* mue_acc_rx_array */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_fill
 *
 *  Purpose   : Receives fill bytes from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_rx_fill
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of fill bytes to receive from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for fill bytes related to a MVB process data port
 *                (size = 0, 16, 24, 28 or 30 bytes).
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_fill
(
    void        *p_bus_ctrl,
    UNSIGNED16   size
)
{
    return mue_acc_rx_fill_idx(p_bus_ctrl, size, DG_DEFAULT_IDX);
}
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_fill_idx
(
    void        *p_bus_ctrl,
    UNSIGNED16   size,
    DG_U8        card_index
)
{
    UNSIGNED16  i;


    /* avoid warnings */
    (void) p_bus_ctrl;
    (void) size;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_fill(size=%d)\n", size);
#endif /* #ifdef MUE_ACC_PRINT */

    for (i=0; i<size; i++)
    {
        dg_card_config[card_index].muac.cch_data_cnt++;
        dg_card_config[card_index].muac.cch_data_ptr++;
    } 

    return(MUE_RESULT_OK);

} /* mue_acc_rx_fill */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_ignore
 *
 *  Purpose   : Ignore surplus data (fill bytes) from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_rx_ignore
 *              (
 *                  void        *p_bus_ctrl,
 *                  UNSIGNED16  size
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *              size       - number of fill bytes to ignore from the UART
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - Used for surplus bytes related to a MVB device status or
 *                a MVB process data port.
 *              - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_ignore
(
    void        *p_bus_ctrl,
    UNSIGNED16  size
)
{

    /* avoid warnings */
    (void) p_bus_ctrl;
    (void) size;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_ignore(size=%d)\n", size);
#endif /* #ifdef MUE_ACC_PRINT */

    return(MUE_RESULT_OK);

} /* mue_acc_rx_ignore */


/* --------------------------------------------------------------------------
 *  Procedure : mue_acc_rx_close
 *
 *  Purpose   : Closes a receive block from the UART.
 *
 *  Syntax    : TCN_DECL_PUBLIC
 *              MUE_RESULT
 *              mue_acc_rx_close
 *              (
 *                  void        *p_bus_ctrl
 *              );
 *
 *  Input     : p_bus_ctrl - pointer to bus controller specific values
 *
 *  Return    : result code; any MUE_RESULT
 *
 *  Remarks   : - A MVB controller is identified by 'p_bus_ctrl'.
 * --------------------------------------------------------------------------
 */
TCN_DECL_PUBLIC
MUE_RESULT
mue_acc_rx_close
(
    void        *p_bus_ctrl
)
{

    /* avoid warnings */
    (void) p_bus_ctrl;

#ifdef MUE_ACC_PRINT
    MUE_OSL_PRINTF("mue_acc_rx_close()\n");
#endif /* #ifdef MUE_ACC_PRINT */

    return(MUE_RESULT_OK);

} /* mue_acc_rx_close */
