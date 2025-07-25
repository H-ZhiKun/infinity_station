/* ==========================================================================
 *
 *  Module      :   TCN_MCS_CPIL.C
 *
 *  Purpose     :   Processor Interface Library
 *                  - access to host interface of MVB interface product
 *                  - access to operating system specific libraries
 *
 *  Project     :   MVB Client-Server Model (MVB Client Driver Software)
 *
 *  Remarks     :   MVB Client Driver:
 *                  - low-level routines in cch.c
 *                  - supports both 32-bit Windows OS and eCos (AT91/D113)
 *                  - static library for both (depends on the compilation
 *                    directory)
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
/*   Compiler Switches                                                        */
/*                                                                            */
/******************************************************************************/
/* O_CPIL_PRINT_ERROR         - makes printout with "printf"                  */
/*                              (in case of an error)                         */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/*   Include Files                                                            */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*   MVB Client Driver Software - Common Interface Definitions                */
/******************************************************************************/
#include "tcn_def.h"

/* --------------------------------------------------------------------------
 *  Communications channel
 * --------------------------------------------------------------------------
 */
#include "dg_hdio.h"
#include "dg_hdc.h"


/******************************************************************************/
/*   Standard Libraries                                                       */
/******************************************************************************/
#include <stdio.h>      /* sprintf      */
#include <string.h>     /* memset       */
#include <stddef.h>

#include "osl.h"        /* osl_malloc, osl_free */

/******************************************************************************/
/*   MVB Client Driver Software                                               */
/******************************************************************************/
#include "tcn_mcs_cpil.h"
#include "tcn_mcs_clch.h"
#include "tcn_mcs_cgf.h"

#ifdef WIN32
/******************************************************************************/
/*   Windows libraries                                                        */
/******************************************************************************/
#include <windows.h>
#endif

/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (begin)                                            */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
extern "C"
{
#endif


/* ==========================================================================
 *
 *  Local Variables - Communication Channel
 *
 * ==========================================================================
 */

/******************************************************************************/
/*                                                                            */
/*   General Definitions                                                      */
/*                                                                            */
/******************************************************************************/



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
#if !defined (O_OS_SINGLE)
static DWORD WINAPI cpil_thread_supervisor (LPVOID p_parameter);
static DWORD WINAPI cpil_thread_callback   (LPVOID p_parameter);
#endif

/******************************************************************************/
/*                                                                            */
/*   Internal Functions                                                       */
/*                                                                            */
/******************************************************************************/

#if !defined (O_OS_SINGLE)

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_thread_supervisor                                     */
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
/*   ABSTRACT:     This function performs the supervisor thread.              */
/*                                                                            */
/******************************************************************************/

static DWORD WINAPI cpil_thread_supervisor (LPVOID p_parameter)
{

    /* avoid warning */
    p_parameter = p_parameter;

    for (;;)
    {

        cgf_supervisor ();

    } /* end "for (;;)" */

} /* end "cpil_thread_supervisor" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_thread_callback                                       */
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
/*   ABSTRACT:     This function performs the callback thread.                */
/*                                                                            */
/******************************************************************************/

static DWORD WINAPI cpil_thread_callback (LPVOID p_parameter)
{
GF_RESULT   result;
BOOLEAN1    bNoCallbacks;
BOOLEAN1    bCallbackBusy;

    /* avoid warning */
    p_parameter = p_parameter;

    for (;;)
    {
        cpil_sem_take(CPIL_SEM_CALLBACK);

        for (;;)
        {
            result = cgf_poll_server_callback(5, &bNoCallbacks, &bCallbackBusy);
            if (result != GF_OK)
            {
#ifdef O_CPIL_PRINT_ERROR
                printf("\nERROR: cpil_thread_callback\n");
#endif
                break;
            } /* if (result != GF_OK) */

            if (bNoCallbacks)
            {
                Sleep(64);
            } /* if (bNoCallbacks) */

        } /* end "for (;;)" */

    } /* end "for (;;)" */

} /* end "cpil_thread_callback" */

#endif /* end "#if !defined (O_OS_SINGLE)" */


/******************************************************************************/
/*                                                                            */
/*   External Functions                                                       */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_init                                                  */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result           CPIL_OK, CPIL_ERROR                       */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function initialises the PIL.                         */
/*                                                                            */
/******************************************************************************/

CPIL_RESULT cpil_init (void)
{
    return cpil_init_idx(DG_DEFAULT_IDX);
}
CPIL_RESULT cpil_init_idx (DG_U8 card_index)
{
CPIL_RESULT         result = CPIL_OK;


#if !defined (O_OS_SINGLE)
    static BOOLEAN1 bInitDone[4] = { FALSE, FALSE, FALSE, FALSE };
    /* semaphores */
    UNSIGNED16      counter;
    /* thread */
    DWORD           thread_supervisor_stack_size;
    DWORD           thread_supervisor_ID;
    DWORD           thread_callback_stack_size;
    DWORD           thread_callback_ID;
#endif

    /* ----------------------------------------------------------------------
     *  initialise FIFO receive buffer
     * ----------------------------------------------------------------------
     */
    dg_card_config[card_index].mscl.p_receive_buffer_first = &dg_card_config[card_index].mscl.receive_buffer[0];
    dg_card_config[card_index].mscl.p_receive_buffer_last  = &dg_card_config[card_index].mscl.receive_buffer[CPIL_RECEICVE_BUFFER_SIZE];
    dg_card_config[card_index].mscl.p_receive_buffer_read  = dg_card_config[card_index].mscl.p_receive_buffer_first;

#if !defined (O_OS_SINGLE)

    /**************************************/
    /* create mutual exclusion semaphores */
    /**************************************/
    if (bInitDone[0] == FALSE)
    {
        for (counter=0; counter<CPIL_MAX_MUTEX; counter++)
        {
            dg_card_config[card_index].mscl.cpil_mutex[counter] = CreateMutex (NULL, FALSE, NULL);
            if (dg_card_config[card_index].mscl.cpil_mutex[counter] == NULL)
            {
                result = CPIL_ERROR_INIT_MUTEX;
                return (result);
            } /* end "if (cpil_mutex[counter] == NULL)" */
        } /* end "for (counter=0; counter<CPIL_MAX_MUTEX; counter++)" */

        bInitDone[0] = TRUE;

    } /* end "if (bInitDone[0] == FALSE)" */

    /*********************/
    /* create semaphores */
    /*********************/
    if (bInitDone[1] == FALSE)
    {
        for (counter=0; counter<CPIL_MAX_SEM; counter++)
        {
            dg_card_config[card_index].mscl.cpil_sem[counter] = CreateSemaphore (NULL, 0, 1, NULL);
            if (dg_card_config[card_index].mscl.cpil_sem[counter] == NULL)
            {
                result = CPIL_ERROR_INIT_SEM;
                return (result);
            } /* end "if (cpil_sem[counter] == NULL)" */
        } /* end "for (counter=0; counter<CPIL_MAX_SEM; counter++)" */

        bInitDone[1] = TRUE;

    } /* end "if (bInitDone[1] == FALSE)" */


    /**************************/
    /* create and start tasks */
    /**************************/

    /*******************/
    /* supervisor task */
    /*******************/
    if (bInitDone[2] == FALSE)
    {
        thread_supervisor_stack_size = 0;
        dg_card_config[card_index].mscl.h_thread_supervisor = CreateThread (NULL, thread_supervisor_stack_size, cpil_thread_supervisor, NULL, 0, &thread_supervisor_ID);
        if (dg_card_config[card_index].mscl.h_thread_supervisor == NULL)
        {
            result = CPIL_ERROR_INIT_TASK_SUPERVISOR;
            return (result);
        } /* end "if (h_thread_supervisor == NULL)" */

        bInitDone[2] = TRUE;

    } /* end "if (bInitDone[2] == FALSE)" */

    /*****************/
    /* callback task */
    /*****************/
    if (bInitDone[3] == FALSE)
    {
        thread_callback_stack_size = 0;
        dg_card_config[card_index].mscl.h_thread_callback = CreateThread (NULL, thread_callback_stack_size, cpil_thread_callback, NULL, 0, &thread_callback_ID);
        if (dg_card_config[card_index].mscl.h_thread_callback == NULL)
        {
            result = CPIL_ERROR_INIT_TASK_CALLBACK;
            return (result);
        } /* end "if (h_thread_callback == NULL)" */

        bInitDone[3] = TRUE;

    } /* end "if (bInitDone[3] == FALSE)" */

#endif /* !O_OS_SINGLE */

    return (result);

} /* end "cpil_init" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_close                                                 */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result           CPIL_OK, CPIL_ERROR                       */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function closes the PIL.                              */
/*                                                                            */
/******************************************************************************/

CPIL_RESULT cpil_close (void)
{
    return cpil_close_idx(DG_DEFAULT_IDX);
}

CPIL_RESULT cpil_close_idx (DG_U8 card_index)
{
#if !defined (O_OS_SINGLE)
    UNSIGNED16  counter;
#else
   (void)card_index;
#endif

#if !defined (O_OS_SINGLE)
    /***************/
    /* close tasks */
    /***************/
    TerminateThread (dg_card_config[card_index].mscl.h_thread_supervisor, 0);
    TerminateThread (dg_card_config[card_index].mscl.h_thread_callback  , 0);
#endif

    /*************************************/
    /* close mutual exclusion semaphores */
    /*************************************/
#if !defined (O_OS_SINGLE)
    for (counter=0; counter<CPIL_MAX_MUTEX; counter++)
    {
        CloseHandle(dg_card_config[card_index].mscl.cpil_mutex[counter]);
    } /* for (counter=0; counter<CPIL_MAX_MUTEX; counter++) */
#endif

    /********************/
    /* close semaphores */
    /********************/
#if !defined (O_OS_SINGLE)
    for (counter=0; counter<CPIL_MAX_SEM; counter++)
    {
        CloseHandle(dg_card_config[card_index].mscl.cpil_sem[counter]);
    } /* for (counter=0; counter<CPIL_MAX_SEM; counter++) */
#endif

    return(CPIL_OK);

} /* end "cpil_close" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_mem_alloc                                             */
/*                                                                            */
/*   INPUT:        mem_size      byte size of memory                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: p_mem         pointer to memory                            */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function allocate memory.                             */
/*                                                                            */
/******************************************************************************/

void * cpil_mem_alloc (UNSIGNED32 mem_size)
{
UNSIGNED32  malloc_size;
UNSIGNED32  *p_memory_32;

    /* calculate "malloc size" */
    if ((mem_size % 4) == 0)
    {
        malloc_size = mem_size;
    }
    else
    {
        malloc_size = mem_size + (4 - (mem_size % 4));
    } /* end "if ((mem_size % 4) == 0)" */

    /* allocate memory */
    p_memory_32 = (UNSIGNED32*) osl_malloc ((size_t)malloc_size);

    return ((void*)p_memory_32);

} /* end "cpil_mem_alloc" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_mem_free                                              */
/*                                                                            */
/*   INPUT:        p_mem         pointer to memory                            */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function releases allocated memory.                   */
/*                                                                            */
/******************************************************************************/

void cpil_mem_free (void * p_mem)
{

    osl_free (p_mem);

} /* end "cpil_mem_free" */


#if !defined (O_OS_SINGLE)

/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_mutex_take                                            */
/*                                                                            */
/*   INPUT:        mutex_number                                               */
/*                                                                            */
/*   OUPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function takes a mutual exclusion semaphore.          */
/*                                                                            */
/******************************************************************************/

void cpil_mutex_take (UNSIGNED16 mutex_number)
{
    DWORD   result;
    DWORD   last_error;

    result = WaitForSingleObject(dg_card_config[card_index].mscl.cpil_mutex[mutex_number], INFINITE);
    if (result != WAIT_OBJECT_0)
    {
        if (result == WAIT_FAILED)
        {
            last_error = GetLastError();
            sprintf(dg_card_config[card_index].mscl.cpil_dbg_str,                                                       \
                "cpil_mutex_take(%d), WaitForSingleObject()=%ld, GetLastError()=%ld\n", \
                    mutex_number, result, last_error);
        }
        else
        {
            sprintf(dg_card_config[card_index].mscl.cpil_dbg_str,                                   \
                "cpil_mutex_take(%d), WaitForSingleObject()=%ld\n", \
                    mutex_number, result);
        } /* if (result == WAIT_FAILED) */
        OutputDebugString((LPCTSTR)dg_card_config[card_index].mscl.cpil_dbg_str);
    } /* if (result != WAIT_OBJECT_0) */

} /* end "cpil_mutex_take" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_mutex_give                                            */
/*                                                                            */
/*   INPUT:        mutex_number                                               */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function gives a mutual exclusion semaphore.          */
/*                                                                            */
/******************************************************************************/

void cpil_mutex_give (UNSIGNED16 mutex_number)
{
    BOOL    result;
    DWORD   last_error;

    result = ReleaseMutex(dg_card_config[card_index].mscl.cpil_mutex[mutex_number]);
    if (result == 0)
    {
        last_error = GetLastError();
        sprintf(dg_card_config[card_index].mscl.cpil_dbg_str,                                               \
            "cpil_mutex_give(%d), ReleaseMutex()=%d, GetLastError()=%ld\n", \
                mutex_number, result, last_error);
        OutputDebugString((LPCTSTR)dg_card_config[card_index].mscl.cpil_dbg_str);
    } /* if (result == 0) */

} /* end "cpil_mutex_give" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_sem_take                                              */
/*                                                                            */
/*   INPUT:        sem_number                                                 */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function takes a semaphore.                           */
/*                                                                            */
/******************************************************************************/

void cpil_sem_take (UNSIGNED16 sem_number)
{

    WaitForSingleObject (dg_card_config[card_index].mscl.cpil_sem[sem_number], INFINITE);

} /* end "cpil_sem_take" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_sem_give                                              */
/*                                                                            */
/*   INPUT:        sem_number                                                 */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: -                                                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function gives a semphore.                            */
/*                                                                            */
/******************************************************************************/

void cpil_sem_give (UNSIGNED16 sem_number)
{

    ReleaseSemaphore (dg_card_config[card_index].mscl.cpil_sem[sem_number], 1, NULL);

} /* end "cpil_sem_give" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_idle_wait                                             */
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
/*   ABSTRACT:     This function performs an idle wait with the minimum       */
/*                 possible time.                                             */
/*                                                                            */
/******************************************************************************/

void cpil_idle_wait (void)
{

    Sleep(0);

} /* end "cpil_idle_wait" */

#endif /* !O_OS_SINGLE */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_start_up_server                                       */
/*                                                                            */
/*   INPUT:        p_device_config     pointer to device configuration        */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              CPIL_OK, CPIL_ERROR                    */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function performs a start up of the MVB server.       */
/*                 If possible, the MVB server hardware will be reset.        */
/*                                                                            */
/******************************************************************************/

CPIL_RESULT cpil_start_up_server (void * p_device_config)
{
    CPIL_RESULT cpil_result;
    cpil_result = CPIL_OK;

    (void)p_device_config;
    return(cpil_result);

} /* end "cpil_start_up_server" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_init_server                                           */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result              CPIL_OK, CPIL_ERROR                    */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function initialises the MVB server.                  */
/*                                                                            */
/******************************************************************************/

CPIL_RESULT cpil_init_server (void)
{
    return cpil_init_server_idx(DG_DEFAULT_IDX);
}
CPIL_RESULT cpil_init_server_idx (DG_U8 card_index)
{
    CPIL_RESULT cpil_result = CPIL_OK;

    /* ----------------------------------------------------------------------
     *  flush the hardware communication buffers
     * ----------------------------------------------------------------------
     */
    cpil_result = cpil_hardware_flush_idx(card_index);
    if (CPIL_OK != cpil_result)
    {
        cpil_result = CPIL_ERROR;
    } /* if (CPIL_OK != cpil_result) */
    else
    {
        if ( DG_OK != hdc_init(card_index) )
            cpil_result = CPIL_ERROR;
    }
    return(cpil_result);

} /* end "cpil_init_server" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_get_server_status                                     */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result     CPIL_OK, CPIL_ERROR                             */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function gets the server status.                      */
/*                                                                            */
/******************************************************************************/

CPIL_RESULT cpil_get_server_status (void)
{

    /* ----------------------------------------------------------------------
     *  Note:
     *  The SERVER cannot indicate a status over this communication channel.
     *  Therefore always return OK.
     * ----------------------------------------------------------------------
     */
    return(CPIL_OK);

} /* end "cpil_get_server_status" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_hardware_flush                                        */
/*                                                                            */
/*   INPUT:        -                                                          */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result     CPIL_OK, CPIL_ERROR                             */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function flush the hardware communication buffers.    */
/*                                                                            */
/******************************************************************************/

CPIL_RESULT cpil_hardware_flush (void)
{
    return cpil_hardware_flush_idx(DG_DEFAULT_IDX);
}
CPIL_RESULT cpil_hardware_flush_idx (DG_U8 card_index)
{
    CPIL_RESULT cpil_result;


    /* ----------------------------------------------------------------------
     *  flush FIFO receive buffer
     * ----------------------------------------------------------------------
     */
    dg_card_config[card_index].mscl.p_receive_buffer_read = dg_card_config[card_index].mscl.p_receive_buffer_first;

    cpil_result = CPIL_OK;

    return(cpil_result);

} /* end "cpil_hardware_flush" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_hardware_send                                         */
/*                                                                            */
/*   INPUT:        p_data        pointer to data                              */
/*                 data_size     number of data bytes                         */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result        CPIL_OK, CPIL_ERROR                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function sends data to the hardware channel.          */
/*                                                                            */
/******************************************************************************/
CPIL_RESULT cpil_hardware_send (UNSIGNED8 * p_data, UNSIGNED16 data_size)
{
    return cpil_hardware_send_idx (p_data, data_size, DG_DEFAULT_IDX);
}
CPIL_RESULT cpil_hardware_send_idx (UNSIGNED8 * p_data, UNSIGNED16 data_size, DG_U8 card_index)
{
    CPIL_RESULT cpil_result = CPIL_OK;
    DG_RESULT hdio_result;


    DG_S32 function_time_out=0;
    DG_S32 function_start_time = osl_get_us_time_stamp();

    if (DG_NO_CARD==card_index)
    {
#ifdef MUE_ACC_PRINT
        MUE_OSL_PRINTF("ERROR: don't know which MVB card to use\n");
#endif
        cpil_result = CPIL_ERROR;
    }
    else
    {
        if ( !(DG_HDIO_IS_INITIALIZED == dg_card_config[card_index].is_initialized) ||
             NO_MVB_CHANNEL == CARD_MVB_SERVER_CHANNEL_NUM(card_index) )
        {
#ifdef MUE_ACC_PRINT    
            MUE_OSL_PRINTF("ERROR: card-index %d not initialised ()\n",card_index);
#endif        
            cpil_result = CPIL_ERROR;
        }
    }
    if ( CPIL_OK == cpil_result ) {
        function_time_out = dg_card_config[card_index].dg_hdio_configs[CARD_MVB_SERVER_CHANNEL_NUM(card_index)].function_timeout;
        /* ----------------------------------------------------------------------
         *  put data to communications channel
         * ----------------------------------------------------------------------
         */
        if ( DG_OK==dg_hdio_wait_to_send(CARD_MVB_SERVER_CHANNEL_NUM(card_index))) {
            hdio_result = dg_hdio_transmit(CARD_MVB_SERVER_CHANNEL_NUM(card_index), data_size, p_data, function_time_out, function_start_time, card_index);
            if ( DG_OK!=hdio_result ) {
                if ( DG_ERROR_TIMEOUT==hdio_result ) {
                    cgf_put_device_status(GF_DEVICE_STATUS_CLIENT_SEND_TIMEOUT, card_index);
                } else {
                    cgf_put_device_status(GF_DEVICE_STATUS_CLIENT_SEND_CHANNEL, card_index);
                }
                cpil_result = CPIL_ERROR;
            }

        } else {
            // time-out waiting for communications channel
            cgf_put_device_status(GF_DEVICE_STATUS_CLIENT_SEND_TIMEOUT, card_index);
            cpil_result = CPIL_ERROR;
        }
    }
    return(cpil_result);

} /* end "cpil_hardware_send" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_wait_for_hardware_receive                             */
/*                                                                            */
/*   INPUT:        number     number of data bytes to receive                 */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result     CPIL_OK, CPIL_ERROR                             */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function waits until a specified number              */
/*                 data bytes are received.                                   */
/*                                                                            */
/******************************************************************************/

CPIL_RESULT cpil_wait_for_hardware_receive (UNSIGNED8 number)
{
    return cpil_wait_for_hardware_receive_idx (number, DG_DEFAULT_IDX);
}
CPIL_RESULT cpil_wait_for_hardware_receive_idx (UNSIGNED8 number, DG_U8 card_index)
{
    CPIL_RESULT cpil_result = CPIL_OK;
    DG_RESULT hdio_result;


    if (DG_NO_CARD==card_index)
    {
        cpil_result = CPIL_ERROR;
    }
    else
    {
    /* ----------------------------------------------------------------------
     *  flush FIFO receive buffer
     * ----------------------------------------------------------------------
     */
        dg_card_config[card_index].mscl.p_receive_buffer_read = dg_card_config[card_index].mscl.p_receive_buffer_first;

    /* ----------------------------------------------------------------------
     *  get data from communication channel
     * ----------------------------------------------------------------------
     */
        hdio_result = dg_hdio_receive(CARD_MVB_SERVER_CHANNEL_NUM(card_index), number, dg_card_config[card_index].mscl.p_receive_buffer_read, card_index);
        if ( DG_OK!=hdio_result ) {
            if ( DG_ERROR_TIMEOUT==hdio_result ) {
                cgf_put_device_status(GF_DEVICE_STATUS_CLIENT_RECEIVE_TIMEOUT, card_index);
            } else {
                cgf_put_device_status(GF_DEVICE_STATUS_CLIENT_RECEIVE_CHANNEL, card_index);
            }
            cpil_result = CPIL_ERROR;
        }

        dg_card_config[card_index].mscl.p_receive_buffer_read = dg_card_config[card_index].mscl.p_receive_buffer_first;
    }
    return(cpil_result);
} /* end "cpil_wait_for_hardware_receive" */


/******************************************************************************/
/*                                                                            */
/*   FUNCTION:     cpil_hardware_receive                                      */
/*                                                                            */
/*   INPUT:        p_data        pointer to data                              */
/*                                                                            */
/*   OUTPUT:       -                                                          */
/*                                                                            */
/*   RETURN VALUE: result        CPIL_OK, CPIL_ERROR                          */
/*                                                                            */
/*   GLOBAL DATA:  -                                                          */
/*                                                                            */
/******************************************************************************/
/*                                                                            */
/*   ABSTRACT:     This function receives a data byte                         */
/*                 from the hardware channel.                                 */
/*                                                                            */
/******************************************************************************/

void cpil_hardware_receive (UNSIGNED8 * p_data)
{
    cpil_hardware_receive_idx (p_data, DG_DEFAULT_IDX);
}
void cpil_hardware_receive_idx (UNSIGNED8 * p_data, DG_U8 card_index)
{


    *p_data = *(dg_card_config[card_index].mscl.p_receive_buffer_read++);

    if (dg_card_config[card_index].mscl.p_receive_buffer_read == dg_card_config[card_index].mscl.p_receive_buffer_last)
    {
        dg_card_config[card_index].mscl.p_receive_buffer_read = dg_card_config[card_index].mscl.p_receive_buffer_first;
    } /* end "if (p_receive_buffer_read == p_receive_buffer_last)" */

} /* end "cpil_hardware_receive" */


/******************************************************************************/
/*                                                                            */
/*   C linkage declaration (end)                                              */
/*                                                                            */
/******************************************************************************/
#if defined(__cplusplus)
}
#endif
