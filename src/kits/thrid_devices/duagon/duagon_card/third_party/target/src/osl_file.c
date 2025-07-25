/* ==========================================================================
 *
 *  File      : OSL_FILE.c
 *
 *  Purpose   : -Host-OS dependent functions for UE2G (file access)
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


/* ==========================================================================
 * These functions must be adapted to the System that this code is running on
 * ==========================================================================
 */

DLL
OSL_FILE* 
osl_file_open_rdonly
(
	DG_CHAR8* path
)
{
	return (OSL_FILE*)fopen(path, "rb");
}

DLL
OSL_FILE*
osl_file_open_writeonly
(
    DG_CHAR8* path
)
{
    return (OSL_FILE*)fopen(path, "wb");
}

DLL
DG_S32 
osl_file_close
(
	OSL_FILE* fd
)
{
	return fclose(fd);
}

DLL
DG_S32
osl_file_seek_set
(
	OSL_FILE* fd,
	DG_U32  offset
) 
{
  return (DG_S32)fseek(fd, offset, SEEK_SET);
}

DLL
DG_S32
osl_file_read
(
	OSL_FILE* fd,
	void* buffer,
	DG_S32 nbyte
) 
{
  return (DG_S32)fread(buffer,1,nbyte,fd); 
}

DLL
DG_S32
osl_file_write
(
    OSL_FILE* fd,
    void* buffer,
    DG_S32 nbyte
)
{
    return (DG_S32)fwrite(buffer,1,nbyte,fd);;
}
