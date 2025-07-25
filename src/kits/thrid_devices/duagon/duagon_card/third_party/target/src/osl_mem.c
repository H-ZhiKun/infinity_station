/* ==========================================================================
 *
 *  File      : OSL_SYNC.c
 *
 *  Purpose   : -Host-OS dependent functions for UE2G (timer and mutex)
 * 
 *
 *  Project   : UE2G Host Driver
 *
 *  Version   :
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

#include <windows.h>

#include <stdlib.h>     /* malloc, free */



/* ==========================================================================
 * These functions must be adapted to the System that this code is running on
 * ==========================================================================
 */
 
void*
osl_malloc
(
   DG_U32 size
)
{
   return malloc ((size_t)size);
}

void
osl_free
(
   void* mem
)
{
   free (mem);
}

void*
osl_realloc                                             \
(                                                   \
    void* mem,
	DG_U32 size
)
{
	return realloc(mem,(size_t)size);
}
