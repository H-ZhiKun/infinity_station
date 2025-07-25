/* ==========================================================================
 *
 *  File      : DG_PFS.H
 *
 *  Purpose   : POSIX file system API
 *
 *  Licence   : Duagon Software Licence (see file 'licence.txt')
 *
 *  Remarks   : !!! DO NOT CHANGE THIS FILE !!!
 *
 * --------------------------------------------------------------------------
 *
 *  (C) COPYRIGHT, Duagon AG, CH-8953 Dietikon, Switzerland
 *  All Rights Reserved.
 *
 * ==========================================================================
 */

#ifndef DG_PFS_H
#define DG_PFS_H

/* ==========================================================================
 *
 *  Documentation
 *
 * ==========================================================================

Introduction
============
The POSIX file system API is the preferred way to exchange data between the
host system and the HTTP or FTP server running on the Duagon interface. Files
written to the Duagon interface board through POSIX can be redirected by the
HTTP or FTP server and data uploaded through a web browser or FTP client can
be read by the host.
This document describes the POSIX file system API, which makes the FLASH file
system on Duagon interface boards accessible over the host interface. The API
supports file and directory functions, and is mostly compatible to the
standard POSIX functions (additional parameters for error handling and real
time handle have been added).

Real time handle
================

For the functions of this API, there exist two ways to call the function:

1) Run to completion: The function returns after the result is completely
evaluated. This can take some time, because first the parameters are
transferred to the device, then the function is executed on the device and
finally the result is transferred back to the host.
-> To run functions to completion, set rtHandle NULL

2) Split transaction: The function returns immediately after the parameters
are transferred to the device. The first call to the function provides a
handle (real time handle). The function must be polled with this handle to
detect if the result of the function is available. The host can do some other
work between polling the function.
-> To run functions with real time handle:
   -initial a RTM_HANDLE variable with rtm_init_handle()
   -repeat calling the function with this handle, until rtm_isFinished()
    returns TRUE
   -evaluate the return parameters

   Example for API XYZ:

   #include <dg_rtm.h>
   RTM_HANDLE rtHandle;
   XYZ_S32 xyz_errno;
   rtm_init_handle(&rtHandle);
   while (1)
   {
       xyz_function(...,...,&xyz_errno,&rtHandle);
	   //TODO: check xyz_errno
       if ( rtm_isFinished(&rtHandle) )
       {
           //TODO: process result
           rtm_init_handle(&rtHandle);
       }
       else
       {
           //TODO: do other activities
       }
   }

Polling Mode
------------
The host interface channels support a polling mode. The polling mode is
useful, if multiple threads access the same host interface channel and
polling is not implemented by the application. If the polling mode is
enabled, a thread can call a non real time handle function, without blocking
other threads from accessing the channel. The channel access functions are
internally implemented with real time handle.

The polling mode can be enabled per channel (e.g. in dg_conf.h):
#define DG_CHANNEL_0_POLLING 1
#define DG_CHANNEL_1_POLLING 1

---------------------------------------------------------------------------*/


/* ==========================================================================
 *
 *  Include Files
 *
 * ==========================================================================
 */
#include "os_def.h"
#include "dg_rtm.h"
#include "dg_conf.h"


/* ==========================================================================
 *
 *  Constants
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  General constants
 * --------------------------------------------------------------------------
 */
#define PFS_PROTOCOL_ID   0x01  /*new protocol id*/

#define PFS_NAME_MAX        64
#define PFS_MAX_PATHLEN    256

/* --------------------------------------------------------------------------
 *  Data types with less than 8-bits
 * --------------------------------------------------------------------------
 */
typedef   DG_BOOL      PFS_BOOL;

/* --------------------------------------------------------------------------
 *  8-bit data types
 * --------------------------------------------------------------------------
 */
typedef   DG_U8        PFS_U8;
typedef   DG_CHAR8     PFS_CHAR8;

/* --------------------------------------------------------------------------
 *  16-bit data types
 * --------------------------------------------------------------------------
 */
typedef   DG_U16       PFS_U16;
typedef   DG_S16       PFS_S16;

/* --------------------------------------------------------------------------
 *  32-bit data types
 * --------------------------------------------------------------------------
 */
typedef   DG_U32       PFS_U32;
typedef   DG_S32       PFS_S32;

/* --------------------------------------------------------------------------
 *  64-bit data types
 * --------------------------------------------------------------------------
 */
typedef   DG_U64       PFS_U64;
typedef   DG_S64       PFS_S64;

/* --------------------------------------------------------------------------
 *  special data types
 * --------------------------------------------------------------------------
 */
typedef DG_U32         PFS_SIZE_T;
typedef DG_S32         PFS_SSIZE_T;

typedef DG_U32         PFS_OFF_T;
typedef DG_U32         PFS_MODE_T;
typedef DG_U32         PFS_INO_T;
typedef DG_S16         PFS_DEV_T;
typedef DG_U16         PFS_NLINK_T;
typedef DG_U16         PFS_UID_T;
typedef DG_U16         PFS_GID_T;
typedef DG_U32         PFS_TIME_T;

typedef OSL_MUTEX      PFS_MUTEX;

typedef DG_U32         PFS_DIR;


typedef struct
{
    PFS_MODE_T  st_mode;     /* File mode */
    PFS_INO_T   st_ino;      /* File serial number */
    PFS_DEV_T   st_dev;      /* ID of device containing file */
    PFS_NLINK_T st_nlink;    /* Number of hard links */
    PFS_UID_T   st_uid;      /* User ID of the file owner */
    PFS_GID_T   st_gid;      /* Group ID of the file's group */
    PFS_OFF_T   st_size;     /* File size (regular files only) */
    PFS_TIME_T  st_atime;    /* Last access time */
    PFS_TIME_T  st_mtime;    /* Last data modification time */
    PFS_TIME_T  st_ctime;    /* Last file status change time */
}   PFS_STAT_STRUCT;

typedef struct
{
    PFS_U8  d_name[PFS_NAME_MAX + 1];
}   PFS_DIRENT_STRUCT;


/* --------------------------------------------------------------------------
 *  errno codes
 * --------------------------------------------------------------------------
 */
#define PFS_ENOERR           0     /* No error */
#define PFS_EPERM            1     /* Not permitted */
#define PFS_ENOENT           2     /* No such entity */
#define PFS_ESRCH            3     /* No such process */
#define PFS_EINTR            4     /* Operation interrupted */
#define PFS_EIO              5     /* I/O error */
#define PFS_EBADF            9     /* Bad file handle */
#define PFS_EAGAIN           11    /* Try again later */
#define PFS_EWOULDBLOCK      PFS_EAGAIN
#define PFS_ENOMEM           12    /* Out of memory */
#define PFS_EBUSY            16    /* Resource busy */
#define PFS_EXDEV            18    /* Cross-device link */
#define PFS_ENODEV           19    /* No such device */
#define PFS_ENOTDIR          20    /* Not a directory */
#define PFS_EISDIR           21    /* Is a directory */
#define PFS_EINVAL           22    /* Invalid argument */
#define PFS_ENFILE           23    /* Too many open files in system */
#define PFS_EMFILE           24    /* Too many open files */
#define PFS_EFBIG            27    /* File too large */
#define PFS_ENOSPC           28    /* No space left on device */
#define PFS_ESPIPE           29    /* Illegal seek */
#define PFS_EROFS            30    /* Read-only file system */
#define PFS_EDOM             33    /* Argument to math function outside valid
                                       domain */
#define PFS_ERANGE           34    /* Math result cannot be represented */
#define PFS_EDEADLK          35    /* Resource deadlock would occur */
#define PFS_EDEADLOCK        PFS_EDEADLK
#define PFS_ENOSYS           38    /* Function not implemented */
#define PFS_ENAMETOOLONG     60    /* File name too long */
#define PFS_ENOTEMPTY        66    /* Directory not empty */
#define PFS_ENOTSUP          95    /* Not supported error */
#define PFS_EEOF             200   /* End of file reached */
#define PFS_ENOSUPP          201   /* Operation not supported */
#define PFS_EDEVNOSUPP       202   /* Device does not support this
                                      operation */
#define PFS_EDATASIZE        203   /* Data size > 10k */

/*
For host driver and communication related errno codes refert to "dg_error.h"
*/


/* --------------------------------------------------------------------------
 *  flags
 * --------------------------------------------------------------------------
 */

/* File access modes used for open() and fnctl() */
#define PFS_O_RDONLY     (1<<0)   /* Open for reading only */
#define PFS_O_WRONLY     (1<<1)   /* Open for writing only */
#define PFS_O_RDWR       (PFS_O_RDONLY|PFS_O_WRONLY) /* Open for reading and
                                                        writing */

/* File access mode mask */
#define PFS_O_ACCMODE    (PFS_O_RDONLY|PFS_O_RDWR|PFS_O_WRONLY)

/* open() mode flags */

#define PFS_O_CREAT      (1<<3)    /* Create file it it does not exist */
#define PFS_O_EXCL       (1<<4)    /* Exclusive use */
#define PFS_O_NOCTTY     (1<<5)    /* Do not assign a controlling terminal */
#define PFS_O_TRUNC      (1<<6)    /* Truncate */

/* File status flags used for open() and fcntl() */
#define PFS_O_APPEND     (1<<7)    /*Set append mode*/
#define PFS_O_DSYNC      (1<<8)    /*Synchronized I/O data integrity writes*/
#define PFS_O_NONBLOCK   (1<<9)    /*No delay*/
#define PFS_O_RSYNC      (1<<10)   /*Synchronized read I/O*/
#define PFS_O_SYNC       (1<<11)   /*Synchronized I/O file integrity writes*/

/* Constants for access() */
#define PFS_F_OK	8

/* Constants for lseek */
#define PFS_SEEK_SET	0
#define PFS_SEEK_CUR	1
#define PFS_SEEK_END	2

/* Variable names for pathconf()	*/
#define	PFS_PC_NAME_MAX		         6
#define	PFS_PC_PATH_MAX		         8

#define PFS_DATA_SIZE_MAX        10240


/* --------------------------------------------------------------------------
 *  Command IDs POSIX
 * --------------------------------------------------------------------------
 */
/* --------------------------------------------------------------------------
 * Commands used to transfer pfs functions on host to duagon device
 */
enum {
	PFS_CMD_CHECK		= 0x00,
	PFS_POSIX_ACCESS    = 0x01,
    PFS_POSIX_OPEN      = 0x02,
    PFS_POSIX_CLOSE     = 0x03,
    PFS_POSIX_READ      = 0x04,
    PFS_POSIX_WRITE     = 0x05,
    PFS_POSIX_LSEEK     = 0x06,
    PFS_POSIX_UNLINK    = 0x07,
    PFS_POSIX_STAT      = 0x08,
    PFS_POSIX_PATHCONF  = 0x09,
    PFS_POSIX_GETCWD    = 0x0A,
    PFS_POSIX_CHDIR     = 0x0B,
    PFS_POSIX_MKDIR     = 0x0C,
    PFS_POSIX_RMDIR     = 0x0D,
    PFS_POSIX_OPENDIR   = 0x0E,
    PFS_POSIX_CLOSEDIR  = 0x0F,
    PFS_POSIX_READDIR_R = 0x10,
    PFS_POSIX_REWINDDIR = 0x11,
    PFS_POSIX_OPEN_READ_CLOSE  =  0x12,
    PFS_POSIX_OPEN_WRITE_CLOSE =  0x13,
    PFS_POSIX_INIT = 0x14,
    PFS_LAST_CMD            /* mark last element */
};


/* ==========================================================================
 *
 *  Macros
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 *  Macros for declaration of variables and procedures.
 *  NOTE:
 *  extern "C" is used in mixed C/C++ headers to force C linkage on an
 *  external definition.
 * --------------------------------------------------------------------------
 */
#define PFS_DECL_VOLATILE   DG_DECL_VOLATILE

#define PFS_DECL_LOCAL      DG_DECL_LOCAL

#ifdef __cplusplus
#   define PFS_DECL_PUBLIC  DG_DECL_PUBLIC
#else
#   define PFS_DECL_PUBLIC  DG_DECL_PUBLIC
#endif

#define PFS_DECL_CONST      DG_DECL_CONST


#ifndef NULL
 #define NULL (void*)0
#endif


/* ==========================================================================
 *
 *  Public Procedure Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 * Procedure :  pfs_init
 *
 * Purpose   :  Initialisation
 *
 * Parameters:  *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_init
(
    PFS_S32      *pfs_errno,
	RTM_HANDLE_P rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_open
 *
 * Purpose   :  Open a file
 *
 * Parameters:  *path       -path of the file (max. 255 characters)
 *              oflag       -values for oflag are constructed by a bitwise-
 *                           inclusive OR of flags from the following list:
 *                             file access modes (mask: PFS_O_ACCMODE):
 *                               -PFS_O_RDONLY   Open for reading only
 *                               -PFS_O_WRONLY   Open for writing only
 *                               -PFS_O_RDWR     Open for reading and writing
 *                             mode flags:
 *                               -PFS_O_CREAT    Create file it it does not
 *                                               exist
 *                               -PFS_O_EXCL     Exclusive use
 *                               -PFS_O_NOCTTY   Do not assign a controlling
 *                                               terminal
 *                               -PFS_O_TRUNC    Truncate
 *                             file status flags:
 *                               -PFS_O_APPEND   Set append mode
 *                               -PFS_O_DSYNC    Synchronized I/O data
 *                                               integrity writes
 *                               -PFS_O_NONBLOCK No delay
 *                               -PFS_O_RSYNC    Synchronized read I/O
 *                               -PFS_O_SYNC     Synchronized I/O file
 *                                               integrity writes
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  non negative file descriptor if successful, otherwise -1
 *              (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_open
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  oflag,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_close
 *
 * Purpose   :  Close a file descriptor
 *
 * Parameters:  fildes      -file descriptor
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_close
(
    PFS_S32      fildes,
	PFS_S32      *pfs_errno,
	RTM_HANDLE_P rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_read
 *
 * Purpose   :  Read from a file
 *
 * Parameters:  fildes      -file descriptor
 *              *buf        -buffer where the data is stored
 *              nbyte       -number of bytes to read (max. 64kB)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  number of bytes read if successful, otherwise -1
 *              (pfs_errno set)
 *
 * Remark    :  Max. data size 10kB
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_SSIZE_T
pfs_read
(
    PFS_S32      fildes,
    void         *buf,
    PFS_SIZE_T   nbyte,
	PFS_S32      *pfs_errno,
	RTM_HANDLE_P rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_open_read_close
 *
 * Purpose   :  Open,read and close
 *
 * Parameters:  path        -path of the file (max. 255 characters)
 *              oflag       -values for oflag are constructed by a bitwise-
 *                           inclusive OR of flags from the following list:
 *                             file access modes (mask: PFS_O_ACCMODE):
 *                               -PFS_O_RDONLY   Open for reading only
 *                               -PFS_O_WRONLY   Open for writing only
 *                               -PFS_O_RDWR     Open for reading and writing
 *                             mode flags:
 *                               -PFS_O_CREAT    Create file it it does not
 *                                               exist
 *                               -PFS_O_EXCL     Exclusive use
 *                               -PFS_O_NOCTTY   Do not assign a controlling
 *                                               terminal
 *                               -PFS_O_TRUNC    Truncate
 *                             file status flags:
 *                               -PFS_O_APPEND   Set append mode
 *                               -PFS_O_DSYNC    Synchronized I/O data
 *                                               integrity writes
 *                               -PFS_O_NONBLOCK No delay
 *                               -PFS_O_RSYNC    Synchronized read I/O
 *                               -PFS_O_SYNC     Synchronized I/O file
 *                                               integrity writes
 *              *buf        -buffer where the data is stored
 *              nbyte       -number of bytes to read (max. 64kB)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :  Max. data size 10kB
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_open_read_close
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  oflag,
    void                     *buf,
    PFS_SIZE_T               nbyte,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_write
 *
 * Purpose   :  Write on a file
 *
 * Parameters:  fildes      -file descriptor
 *              *buf        -pointer to the data to write
 *              nbyte       -number of bytes to write (max. 64kB)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  number of bytes written if successful, otherwise -1
 *              (pfs_errno set)
 *
 * Remark    :  Max. data size 10kB
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_SSIZE_T
pfs_write
(
    PFS_S32             fildes,
    PFS_DECL_CONST void *buf,
    PFS_SIZE_T          nbyte,
	PFS_S32             *pfs_errno,
	RTM_HANDLE_P        rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_open_write_close
 *
 * Purpose   :  Open, write and close
 *
 * Parameters:  path        -path of the file (max. 255 characters)
 *              oflag       -values for oflag are constructed by a bitwise-
 *                           inclusive OR of flags from the following list:
 *                             file access modes (mask: PFS_O_ACCMODE):
 *                               -PFS_O_RDONLY   Open for reading only
 *                               -PFS_O_WRONLY   Open for writing only
 *                               -PFS_O_RDWR     Open for reading and writing
 *                             mode flags:
 *                               -PFS_O_CREAT    Create file it it does not
 *                                               exist
 *                               -PFS_O_EXCL     Exclusive use
 *                               -PFS_O_NOCTTY   Do not assign a controlling
 *                                               terminal
 *                               -PFS_O_TRUNC    Truncate
 *                             file status flags:
 *                               -PFS_O_APPEND   Set append mode
 *                               -PFS_O_DSYNC    Synchronized I/O data
 *                                               integrity writes
 *                               -PFS_O_NONBLOCK No delay
 *                               -PFS_O_RSYNC    Synchronized read I/O
 *                               -PFS_O_SYNC     Synchronized I/O file
 *                                               integrity writes
 *              *buf        -buffer where the data is stored
 *              nbyte       -number of bytes to read (max. 64kB)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :  Max. data size 10kB
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_open_write_close
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  oflag,
    PFS_DECL_CONST void      *buf,
    PFS_SIZE_T               nbyte,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_unlink
 *
 * Purpose   :  Remove a directory entry
 *
 * Parameters:  *path       -path of the file (max. 255 characters)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_unlink
(
    PFS_DECL_CONST PFS_CHAR8 *path,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_access
 *
 * Purpose   :  Determine accessability of a file
 *
 * Parameters:  *path       -path of the file (max. 255 characters)
 *              amode       -flags (only PFS_F_OK supported)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_access
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  amode,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_chdir
 *
 * Purpose   :  Change working directory
 *
 * Parameters:  *path       -path of the directory (max. 255 characters)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :  only change to next sub level possible
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_chdir
(
    PFS_DECL_CONST PFS_CHAR8 *path,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_rmdir
 *
 * Purpose   :  Remove a directory
 *
 * Parameters:  *path       -path of the directory (max. 255 characters)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :  only deletion of directory in CWD possible
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_rmdir
(
    PFS_DECL_CONST PFS_CHAR8 *path,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_mkdir
 *
 * Purpose   :  Make a directory
 *
 * Parameters:  *path       -path of the file (max. 255 characters)
 *              mode        -set 0, not supported (only included for
 *                           compatibility reasons)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :  only creation of directory in CWD possible
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_mkdir
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  mode,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_opendir
 *
 * Purpose   :  Open a directory
 *
 * Parameters:  *path       -path of the directory (max. 255 characters)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  "pointer" to DIR object if successful, otherwise -1
 *              (pfs_errno set)
 *
 * Remark    :   only directories in the CWD can be opened
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_DIR
pfs_opendir
(
    PFS_DECL_CONST PFS_CHAR8 *path,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_closedir
 *
 * Purpose   :  Close a directory stream
 *
 * Parameters:  dirp        -"pointer" to DIR object
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_closedir
(
    PFS_DIR      dirp,
	PFS_S32      *pfs_errno,
	RTM_HANDLE_P rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_rewinddir
 *
 * Purpose   :  Reset the position of a directory stream to the beginning of
 *              a directory
 *
 * Parameters:  dirp        -"pointer" to DIR object
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_rewinddir
(
    PFS_DIR      dirp,
	PFS_S32      *pfs_errno,
	RTM_HANDLE_P rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_getcwd
 *
 * Purpose   :  Get the pathname of the current working directory
 *
 * Parameters:  *buf        -buffer for the path string
 *              size        -size of the buffer (has to be >255)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  pointer to buf if successful, otherwise NULL (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_CHAR8*
pfs_getcwd
(
    PFS_CHAR8    *buf,
    PFS_SIZE_T   size,
	PFS_S32      *pfs_errno,
	RTM_HANDLE_P rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_lseek
 *
 * Purpose   :  Move the read/write file offset
 *
 * Parameters:  fildes      -file descriptor
 *              offset      -offset in bytes (base depends on whence)
 *              whence      -flag that indicates the offset base:
 *                             PFS_SEEK_SET set offset from beginning of file
 *                             PFS_SEEK_CUR set offset from current location
 *                             PFS_SEEK_END set file pointer to the end of
 *                                          the file (Note: offset always 0) 
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  resulting offset from the beginning of the file if
 *              successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_OFF_T
pfs_lseek
(
    PFS_S32      fildes,
    PFS_OFF_T    offset,
    PFS_S32      whence,
	PFS_S32      *pfs_errno,
	RTM_HANDLE_P rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_pathconf
 *
 * Purpose   :  Get configurable pathname variables
 *
 * Parameters:  *path       -path of the file or directory (max. 255
 *                           characters)
 *              name        -variable to be queried relative to that file or
 *                           directory (only PFS_PC_NAME_MAX and
 *                           PFS_PC_PATH_MAX supported)
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  current varible value if successful, otherwise -1 (pfs_errno
 *              set)
 *
 * Remark    :  only applicable to objects in the CWD
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S64
pfs_pathconf
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  name,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_stat
 *
 * Purpose   :  Get file status
 *
 * Parameters:  *path       -path of the file (max. 255 characters)
 *              *buf        -pointer to PFS_STAT_STRUCT object
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :  only applicable to objects in the CWD
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_stat
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_STAT_STRUCT          *buf,
	PFS_S32                  *pfs_errno,
	RTM_HANDLE_P             rtHandle
);


/* --------------------------------------------------------------------------
 * Procedure :  pfs_readdir_r
 *
 * Purpose   :  Read a directory
 *
 * Parameters:  dirp        -"pointer" (only valid on device) to DIR object
 *              *entry      -pointer to PFS_DIRENT_STRUCT object
 *              **result    -indicates result
 *              *pfs_errno  -pointer to error code variable
 *              rtHandle    -see chapter "Real time handle" at the beginning
 *                           of this file
 *
 * Return    :  0 if successful, otherwise -1 (pfs_errno set)
 *
 * Remark    :
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_readdir_r
(
    PFS_DIR           *dirp,
    PFS_DIRENT_STRUCT *entry,
    PFS_DIRENT_STRUCT **result,
	PFS_S32           *pfs_errno,
	RTM_HANDLE_P      rtHandle
);

#endif /* DG_PFS_H */

