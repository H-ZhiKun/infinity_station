/* ==========================================================================
 *
 *  File      : OSL.H
 *
 *  Purpose   : Implements a Hardware near Operating System dependent layer
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 *  Remarks   : This file is inteded to provide a layer that is also
 *              compatible with Streaming media like USB or Ethernet
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon AG, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */

#ifndef OSL_H
#define OSL_H


#include "os_def.h"
#include "dg_error.h"
#include "dg_conf.h"

DG_DECL_PUBLIC
DG_BOOL
osl_check_timer
(
    OSL_TIMER   *timer,
    DG_U32    timeout
);


DG_DECL_PUBLIC
void
osl_start_timer
(
    OSL_TIMER    *timer
);


DG_DECL_PUBLIC
DG_RESULT
osl_init_uart_mutex
(
    OSL_MUTEX     *mutex
);


DG_DECL_PUBLIC
DG_RESULT
osl_lock_uart_mutex
(
    OSL_MUTEX   *mutex
);


DG_DECL_PUBLIC
DG_RESULT
osl_unlock_uart_mutex
(
    OSL_MUTEX   *mutex
);


DG_DECL_PUBLIC
DG_RESULT
osl_destroy_uart_mutex
(
    OSL_MUTEX   *mutex
);


DG_DECL_PUBLIC
void*
osl_malloc
(
    DG_U32    size
);


DG_DECL_PUBLIC
void*
osl_realloc
(
    void*       mem,
    DG_U32    size
);


DG_DECL_PUBLIC
void
osl_free
(
    void* mem
);


DG_DECL_PUBLIC
OSL_FILE* 
osl_file_open_rdonly
(
	DG_CHAR8* path
);


DG_DECL_PUBLIC
OSL_FILE*
osl_file_open_writeonly
(
    DG_CHAR8* path
);


DG_DECL_PUBLIC
DG_S32
osl_file_close
(
	OSL_FILE* fd
);


DG_DECL_PUBLIC
DG_S32 
osl_file_seek_set
(
	OSL_FILE* fd,
	DG_U32  offset
);


DG_DECL_PUBLIC
DG_S32
osl_file_read
(
	OSL_FILE* fd,
	void* buffer,
	DG_S32 nbyte
);


DG_DECL_PUBLIC
DG_S32
osl_get_us_time_stamp
(
	void
);

#endif /* OSL_H */
