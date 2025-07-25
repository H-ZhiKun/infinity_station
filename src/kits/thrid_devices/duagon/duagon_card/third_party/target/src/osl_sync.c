/* ==========================================================================
 *
 *  File      : OSL_SYNC.c
 *
 *  Purpose   : Host-OS dependent timer and mutex functions
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
 *  Include Files and definitions
 *
 * ==========================================================================
 */
#include "os_def.h"
#include "dg_hdio.h"

#include <windows.h>

#if (1 == HAVE_PRINTF)
#include <stdio.h>     /* printf        */
#include <stdarg.h>    /* vprintf       */
#endif

#if (1 == OSL_DEBUG)
#define osl_db_printf osl_printf
#else
#define osl_db_printf dummy_printf
DG_DECL_LOCAL void dummy_printf(DG_DECL_CONST DG_CHAR8* nothing,...) { nothing = nothing; } /* dummy printf to avoid warnings */
#endif

void do_ten_seconds(void);
/* ==========================================================================
 * These functions must be adapted to the System that this code is running on
 * ==========================================================================
 */


/* --------------------------------------------------------------------------
 * Procedure :  osl_start_timer
 *
 * Purpose   :  OS implementation to start a timer
 *
 * Input     :  timer         - pointer to a timer structure that has to be
 *                              defined as OSL_TIMER
 *
 * Return    :
 * --------------------------------------------------------------------------
 */
DG_DECL_PUBLIC
void
osl_start_timer
(
    OSL_TIMER    *timer
)
{
#if (1 == HAVE_TIMER)
    (*timer) = (OSL_TIMER)time(0);
#else
    timer = timer;
#endif
} /* osl_start_timer */

/* --------------------------------------------------------------------------
 * Procedure :  osl_check_timer
 *
 * Purpose   :  OS dependent Timer Implementation. Checks if timer exceeded.
 *
 * Input     :  timer         - pointer to a timer structure that has to be
 *                              defined as OSL_TIMER
 *              timeout       - timeout in milli seconds
 *
 * Return    :  false if timeout is exceeded, otherwise true
 * --------------------------------------------------------------------------
 */
DG_DECL_PUBLIC
DG_BOOL
osl_check_timer
(
    OSL_TIMER   *timer,
    DG_U32    timeout
)
{
    DG_BOOL   result = TRUE;

#if (1 == HAVE_TIMER)
    OSL_TIMER   timestamp;

    timestamp = time(0);
    if ( timestamp > ((*timer) + (OSL_TIMER)(timeout/1000)) )
    {
        result = FALSE;
    }
#else
    timer = timer;
    timeout = timeout;
#endif /*HAVE_TIMER*/

    return result;
} /* osl_check_timer */


/* ==========================================================================
 * Mutex functions
 */

/* --------------------------------------------------------------------------
 * Procedure :  osl_init_uart_mutex
 *
 * Purpose   :  Allocates the memory and initializes a mutex
 *
 * Input     :  mutex         -pointer of the pointer to the mutex
 *
 * Return    :  DG_OK if successful, else any DG_ERROR
 * --------------------------------------------------------------------------
 */
DLL
DG_DECL_PUBLIC
DG_RESULT
osl_init_uart_mutex
(
    OSL_MUTEX     *mutex
)
{
	DG_S32 result = 0;
    DG_S32 mutex_result = 0;

    osl_db_printf("- osl_init_uart_mutex(0x%08X)\n", (DG_U32)mutex);

#if (1 == HAVE_MUTEX)
    if ( NULL == mutex)
    {
        osl_db_printf("- osl_init_uart_mutex(): NULL-Pointer\n");
        result = DG_ERROR_OSL_SYNC_INIT;
    }

    if (0 == result)
    {
        mutex_result = (DG_S32)pthread_mutex_init(mutex, NULL);
        osl_db_printf("- mutex_result = %d\n", mutex_result);
        if (0 != mutex_result)
        {
            osl_db_printf("- osl_init_uart_mutex(): pthread_mutex_init \
                           returns %d\n", mutex_result);
            result = DG_ERROR_OSL_SYNC_INIT;
        }
    }
    osl_db_printf("- pthread_mutex_init()=%d\n", result);
    if (0 != result)
    {
        osl_db_printf("- pthread_mutex_init()=%d\n", result);
    }
#else
    mutex = mutex;
#endif /*HAVE _MUTEX*/

    osl_db_printf("- osl_init_uart_mutex(0x%08X)=%d\n", (DG_U32)mutex, result);

    return result;
} /* osl_init_uart_mutex */


/* --------------------------------------------------------------------------
 * Procedure :  osl_lock_uart_mutex
 *
 * Purpose   :  Wait for mutex
 *
 * Input :      mutex         -pointer to the mutex
 *
 * Return :     DG_OK if successful, else any DG_ERROR
 * --------------------------------------------------------------------------
 */
DLL
DG_DECL_PUBLIC
DG_RESULT
osl_lock_uart_mutex
(
    OSL_MUTEX   *mutex
)
{
    DG_S32 result = 0;
    DG_S32 mutex_result = 0;

    osl_db_printf("- osl_lock_uart_mutex(0x%08X)\n", (DG_U32)mutex);

#if (1 == HAVE_MUTEX)
    if ( NULL == mutex || NULL == *mutex)
    {
        osl_db_printf("- osl_lock_uart_mutex(): NULL-Pointer\n");
        result = DG_ERROR_OSL_SYNC_LOCK;
    }

    if (0 == result)
    {
        mutex_result = (DG_S32)pthread_mutex_lock((pthread_mutex_t*)mutex);
        if (0 != mutex_result)
        {
            osl_db_printf("- osl_lock_uart_mutex(): pthread_mutex_lock \
                           returns %d\n", mutex_result);
            result = DG_ERROR_OSL_SYNC_LOCK;
        }
    }

    if (0 != result)
    {
        osl_db_printf("- pthread_mutex_lock()=%d\n", result);
    }
#else
    mutex = mutex;
#endif /*HAVE _MUTEX*/

    osl_db_printf("- osl_lock_uart_mutex()=%d\n", result);

    return result;
} /* osl_lock_uart_mutex */


/* --------------------------------------------------------------------------
 * Procedure :  osl_unlock_uart_mutex
 *
 * Purpose :    Unlock mutex
 *
 * Input :      mutex        -pointer to the mutex
 *
 * Return :     DG_OK if successful, else any DG_ERROR
 * --------------------------------------------------------------------------
 */
DLL
DG_DECL_PUBLIC
DG_RESULT
osl_unlock_uart_mutex
(
    OSL_MUTEX   *mutex
)
{
    DG_S32 result = 0;
    DG_S32 mutex_result = 0;

    osl_db_printf("- osl_unlock_uart_mutex(0x%08X)\n", (DG_U32)mutex);

#if (1 == HAVE_MUTEX)
    if ( NULL == mutex  || NULL == *mutex)
    {
        osl_db_printf("- osl_unlock_uart_mutex(): NULL-Pointer\n");
        result = DG_ERROR_OSL_SYNC_UNLOCK;
    }

    if (0 == result)
    {
        mutex_result = (DG_S32)pthread_mutex_unlock                       \
                                 ((pthread_mutex_t*)mutex);
        if (0 != mutex_result)
        {
            osl_db_printf("- osl_unlock_uart_mutex(): pthread_mutex_unlock \
                           returns %d\n", mutex_result);
            result = DG_ERROR_OSL_SYNC_UNLOCK;
        }
    }

    if (0 != result)
    {
        osl_db_printf("- pthread_mutex_unlock()=%d\n", result);
    }
#else
    mutex = mutex;
#endif /*HAVE _MUTEX*/

    osl_db_printf("- osl_unlock_uart_mutex()=%d\n", result);

    return result;
} /* osl_unlock_uart_mutex */

/* --------------------------------------------------------------------------
 * Procedure :  osl_destroy_uart_mutex
 *
 * Purpose :    Destroy mutex
 *
 * Input :      mutex        -pointer to the mutex
 *
 * Return :     DG_OK if successful, else any DG_ERROR
 * --------------------------------------------------------------------------
 */
DLL
DG_DECL_PUBLIC
DG_RESULT
osl_destroy_uart_mutex
(
    OSL_MUTEX   *mutex
)
{
    DG_S32 result = 0;
    DG_S32 mutex_result = 0;

    osl_db_printf("- osl_destroy_uart_mutex(0x%08X)\n", (DG_U32)mutex);

#if (1 == HAVE_MUTEX)
    if ( NULL == mutex  || NULL == *mutex)
    {
        osl_db_printf("- osl_destroy_uart_mutex(): NULL-Pointer\n");
        result = DG_ERROR_OSL_SYNC_DESTROY;
    }

    if (0 == result)
    {
        mutex_result = (DG_S32)pthread_mutex_destroy                      \
                                 ((pthread_mutex_t*)mutex);
        if (0 != mutex_result)
        {
            osl_db_printf("- osl_destroy_uart_mutex(): pthread_mutex_destroy\
                            returns %d\n", mutex_result);
            result = DG_ERROR_OSL_SYNC_DESTROY;
        }
    }

    if (0 != result)
    {
        osl_db_printf("- pthread_mutex_destroy()=%d\n", result);
    }
#else
    mutex = mutex;
#endif /*HAVE _MUTEX*/

    osl_db_printf("- osl_destroy_uart_mutex()=%d\n", result);

    return result;
} /* osl_destroy_uart_mutex */

/* --------------------------------------------------------------------------
 * Procedure :  osl_get_us_time_stamp
 *
 * Purpose :    deliver a count increasing in microseconds.
 *              it counts circularly through 0.
 *
 * Return :     DG_U32 result = count in microseconds
 * --------------------------------------------------------------------------
 */
DLL
DG_DECL_PUBLIC
DG_S32
osl_get_us_time_stamp(void)
{
    LARGE_INTEGER frequency; /*counts per second*/
    LARGE_INTEGER count;
    DG_U32 result;
    DG_U64 freq64, count64;

    osl_db_printf( "- osl_get_us_time_stamp \n" );
    result = (DG_U32)QueryPerformanceFrequency(&frequency);
    if ( result != 0 )
    {
        freq64 =  frequency.HighPart;
        freq64 = (freq64 << 32) + frequency.LowPart;
    	result = (DG_U32)QueryPerformanceCounter(&count);
    	if ( result != 0 )
    	{
    		count64 = count.HighPart;
    		count64 = (count64 << 32) + count.LowPart;
    		result = (DG_S32)((count64 / (freq64 / 1000000)) & 0x00000000FFFFFFFF);
    	}
    }
	return result;
}

/* --------------------------------------------------------------------------
 * Procedure :  do_ten_seconds
 *
 * Purpose :    to test osl_get_us_time_stamp()
 *
 * --------------------------------------------------------------------------
 */
void
do_ten_seconds(void)
{
	DG_S32 count, res;
	char i;

	for (i=0; i<10; i++)
	{
		count = osl_get_us_time_stamp();
		res = count;
		putchar('.');
		while ( res - count < 1000000 )
		{
			res = osl_get_us_time_stamp();
		}
	}
	putchar('\n');
}


