/* ==========================================================================
 *
 *  File      : DG_PFS.C
 *
 *  Purpose   : POSIX file system API
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
 *  Include Files
 *
 * ==========================================================================
 */
#include "dg_pfs.h"
#include "dg_rpc.h"
#include "dg_hdc.h"
#include "dg_rtm.h"

/* ==========================================================================
 *  Defines
 * ==========================================================================
 */

/* ==========================================================================
 *
 *  Posix file system driver version
 *
 * ==========================================================================
 */
#define PFS_DRIVER_VERSION 0x00010001 /* 0xMMMMmmmm MMMM=major=1 mmmm=minor=1 */

/* ==========================================================================
 *
 *  Posix file system protocol version
 *
 * ==========================================================================
 */
#define PFS_MIN_PROT_VERSION_MAJOR 1 /* POSIX protocol on the device must have at least this major value */
#define PFS_MIN_PROT_VERSION_MINOR 1 /* POSIX protocol on the device must have at least this minor value */

/* ==========================================================================
 *
 *  printf definition
 *
 * ==========================================================================
 */

#if (1 == PFS_DEBUG)
#define pfs_debug_printf osl_printf
#else
#define pfs_debug_printf dummy_printf
PFS_DECL_LOCAL void dummy_printf(DG_DECL_CONST PFS_CHAR8* nothing,...) { nothing=nothing; } /* dummy printf to avoid warnings */
#endif

/* ==========================================================================
 *
 *  Public Procedure Interface
 *
 * ==========================================================================
 */

/* --------------------------------------------------------------------------
 * pfs_open
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
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;

    pfs_debug_printf("pfs_open(%s, ...)\n", path);
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_OPEN,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_32, &oflag,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return != 0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_open(%s, ...) = %d (pfs_errno=%d)\n", path, retVal, *pfs_errno);
    return retVal;

}

/* --------------------------------------------------------------------------
 * pfs_close
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_close
(
    PFS_S32      fildes,
    PFS_S32      *pfs_errno,
    RTM_HANDLE_P rtHandle
)
{
    PFS_S32 retVal=0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_close()\n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_CLOSE,
                 rtHandle,
                 RPC_32, &fildes,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_close() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}


/* --------------------------------------------------------------------------
 * pfs_read
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_SSIZE_T
pfs_read
(
    PFS_S32     fildes,
    void        *buf,
    PFS_SIZE_T  nbyte,
    PFS_S32     *pfs_errno,
    RTM_HANDLE_P rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_read()\n");

    if (PFS_DATA_SIZE_MAX < nbyte)
    {
        *pfs_errno = PFS_EDATASIZE;
        return -1;
    }

    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_READ,
                 rtHandle,
                 RPC_32, &fildes,
                 RPC_RETURN_ARRAY8, buf, &nbyte,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_read() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}


/* --------------------------------------------------------------------------
 * pfs_open_read_close
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
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_open_read_close() \n");

    if (PFS_DATA_SIZE_MAX < nbyte)
    {
        *pfs_errno = PFS_EDATASIZE;
        return -1;
    }

    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_OPEN_READ_CLOSE,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_32, &oflag,
                 RPC_RETURN_ARRAY8, buf, &nbyte,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
       retVal=-1;
       *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_open_read_close() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;

}


/* --------------------------------------------------------------------------
 * pfs_write
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
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_write()\n");

    if (PFS_DATA_SIZE_MAX < nbyte)
    {
        *pfs_errno = PFS_EDATASIZE;
        return -1;
    }

    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_WRITE,
                 rtHandle,
                 RPC_32, &fildes,
                 RPC_ARRAY8, buf, &nbyte,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_write() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}


/* --------------------------------------------------------------------------
 * pfs_open_write_close
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
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_open_write_close()\n");

    if (PFS_DATA_SIZE_MAX < nbyte)
    {
        *pfs_errno = PFS_EDATASIZE;
        return -1;
    }

    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_OPEN_WRITE_CLOSE,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_32, &oflag,
                 RPC_ARRAY8, buf, &nbyte,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_open_write_close() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}


/* --------------------------------------------------------------------------
 * pfs_unlink
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_unlink
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  *pfs_errno,
    RTM_HANDLE_P             rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_unlink() \n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_UNLINK,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if(rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_unlink() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_init
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_init
(
    PFS_S32      *pfs_errno,
    RTM_HANDLE_P rtHandle
)
{
    PFS_S32 rpc_return = 0;
    PFS_S32 ret_val = 0;
    RPC_S32 version=PFS_DRIVER_VERSION;

    pfs_debug_printf("pfs_init() [singular]\n");
    if ( rtHandle!=NULL )
    {
        ret_val = hdc_init(rtHandle->card_index);
    }
    else
    {
        ret_val = hdc_init(0);
    }
    if (ret_val != DG_OK)
    {
        if (rtHandle) rtm_setFinished(rtHandle);
        *pfs_errno = ret_val;
        return -1;
    }
    rpc_return = dg_rpc(
        PFS_PROTOCOL_ID,
        PFS_POSIX_INIT,
        rtHandle,
        RPC_IO_32, &version,
        RPC_RETURN_32, &ret_val,
        RPC_RETURN_32, pfs_errno,
        RPC_END);
    if ( rpc_return != 0 )
    {
        *pfs_errno = rpc_return;
        ret_val=-1;
    }
    if (ret_val != 0)
        return ret_val;

    if ( ((version >> 16)& 0x0000FFFF) < PFS_MIN_PROT_VERSION_MAJOR || (version & 0x0000FFFF) < PFS_MIN_PROT_VERSION_MINOR  )
    {
        *pfs_errno = DG_ERROR_VERSION_DEVICE_OLD;
        ret_val=-1;
    }
    return ret_val;
}

/* --------------------------------------------------------------------------
 * pfs_access
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
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_access()\n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_ACCESS,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_32, &amode,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_access() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_chdir
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_chdir
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  *pfs_errno,
    RTM_HANDLE_P             rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_chdir(%s) \n", path);
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_CHDIR,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_chdir(%s) = %d (pfs_errno=%d)\n", path, retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_rmdir
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_rmdir
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  *pfs_errno,
    RTM_HANDLE_P             rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_rmdir()\n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_RMDIR,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_rmdir() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_mkdir
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
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_mkdir() \n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_MKDIR,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_32, &mode,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_mkdir() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_opendir
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_DIR
pfs_opendir
(
    PFS_DECL_CONST PFS_CHAR8 *path,
    PFS_S32                  *pfs_errno,
    RTM_HANDLE_P             rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_opendir(%s)\n", path);
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_OPENDIR,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_opendir(%s) dirPointer= 0x%04X (pfs_errno=%d)\n", path, retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_closedir
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_closedir
(
    PFS_DIR     dirp,
    PFS_S32     *pfs_errno,
    RTM_HANDLE_P rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_closedir()\n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_CLOSEDIR,
                 rtHandle,
                 RPC_32, &dirp,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_closedir() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_rewinddir
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_S32
pfs_rewinddir
(
    PFS_DIR     dirp,
    PFS_S32     *pfs_errno,
    RTM_HANDLE_P rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_rewinddir()\n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_REWINDDIR,
                 rtHandle,
                 RPC_32, &dirp,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_rewinddir() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_getcwd
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_CHAR8*
pfs_getcwd
(
    PFS_CHAR8  *buf,
    PFS_SIZE_T size,
    PFS_S32    *pfs_errno,
    RTM_HANDLE_P rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_getcwd()\n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_GETCWD,
                 rtHandle,
                 RPC_RETURN_ARRAY8, buf, &size,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_getcwd() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal < 0 ? NULL : buf;
}

/* --------------------------------------------------------------------------
 * pfs_lseek
 * --------------------------------------------------------------------------
 */
PFS_DECL_PUBLIC
PFS_OFF_T
pfs_lseek
(
    PFS_S32    fildes,
    PFS_OFF_T  offset,
    PFS_S32    whence,
    PFS_S32    *pfs_errno,
    RTM_HANDLE_P rtHandle
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_lseek()\n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_LSEEK,
                 rtHandle,
                 RPC_32, &fildes,
                 RPC_32, &offset,
                 RPC_32, &whence,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_lseek() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_pathconf
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
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_pathconf()\n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_PATHCONF,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_32, &name,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_pathconf() = %d (pfs_errno=%d)\n", retVal, *pfs_errno);
    return (PFS_S64)retVal;
}

/* --------------------------------------------------------------------------
 * pfs_stat
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
)
{
    PFS_S32 retVal = 0;
    PFS_S32 rpc_return = 0;
    pfs_debug_printf("pfs_stat() \n");
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_STAT,
                 rtHandle,
                 RPC_TEXT, path,
                 RPC_RETURN_32, &(buf->st_mode),
                 RPC_RETURN_32, &(buf->st_ino),
                 RPC_RETURN_16, &(buf->st_dev),
                 RPC_RETURN_16, &(buf->st_nlink),
                 RPC_RETURN_16, &(buf->st_uid),
                 RPC_RETURN_16, &(buf->st_gid),
                 RPC_RETURN_32, &(buf->st_size),
                 RPC_RETURN_32, &(buf->st_atime),
                 RPC_RETURN_32, &(buf->st_mtime),
                 RPC_RETURN_32, &(buf->st_ctime),
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if (rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }
    pfs_debug_printf("pfs_stat() = %d (pfs_errno=%d)\n",retVal, *pfs_errno);
    return retVal;
}

/* --------------------------------------------------------------------------
 * pfs_readdir_r
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
)
{
    PFS_S32 path_size = PFS_NAME_MAX + 1;
    PFS_S32 retVal = 0;
    PFS_U8 zero_when_end;
    PFS_S32 rpc_return = 0;

    pfs_debug_printf("pfs_readdir_r(0x%04X)\n", *dirp);
    rpc_return = dg_rpc(
                 PFS_PROTOCOL_ID,
                 PFS_POSIX_READDIR_R,
                 rtHandle,
                 RPC_32, dirp,
                 RPC_RETURN_ARRAY8, &(entry->d_name),&path_size,
                 RPC_RETURN_8 , &zero_when_end,
                 RPC_RETURN_32, &retVal,
                 RPC_RETURN_32, pfs_errno,
                 RPC_END);
    if(rpc_return!=0)
    {
        retVal=-1;
        *pfs_errno = rpc_return;
    }

    pfs_debug_printf("pfs_readdir(%s) = %d (pfs_errno=%d)\n", entry->d_name, retVal, *pfs_errno);
    if (zero_when_end == 0) {
        *result = NULL;
    } else {
        *result = entry;
    }
    return retVal;

}
