/* ==========================================================================
 *
 *  File      : OSL_PAR.c
 *
 *  Purpose   : -Host-OS dependent functions for UE2G (Parallel REG access)
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
#include "dg_hdreg.h"
#include "dg_reg1g.h"
#include "dg_reg2g.h"
#include "dg_reg3g.h"
#include "dg_error.h"

#include <stdio.h>

#include <windows.h>
#if IO_LIB == OLS
#   include "OlsApiInit.h"
#elif IO_LIB == GIVEIO
#   include "dg_pci.h"
#endif


/* Declaration of _inp / _outp to avoid warnings */
#if IO_LIB == GIVEIO
DG_S32 _outp(DG_U16 port, DG_S32 databyte);
DG_S32 _inp(DG_U16 port);
#endif

#if (1 == DB_REG_ACCESS)
#define osl_debug_printf osl_printf
#else
#define osl_debug_printf dummy_printf
DG_DECL_LOCAL void dummy_printf(DG_DECL_CONST DG_CHAR8* nothing,...) { nothing = nothing; } /* dummy printf to avoid warnings */
#endif

HMODULE hModule;

DG_DECL_PUBLIC
void
osl_par_hal_write8
(
        REG_SIZE* addr,
        DG_U8 val
)
{
    osl_debug_printf("Write: 0x%08X - 0x%04X\n",(DG_U32)addr,val);
#   if IO_LIB == OLS
        WriteIoPortByte((WORD)(DG_U32)addr, val);
#   else
        _outp((DG_U16)(DG_U32)addr, (DG_S32)val);
#   endif
}

DG_DECL_PUBLIC
void
osl_par_hal_write32
(
        REG_SIZE* addr,
        DG_U32 val
)
{
    osl_debug_printf("Write: 0x%08X - 0x%08X\n",(DG_U32)addr,val);
#   if IO_LIB == OLS
        WriteIoPortByte((WORD)(DG_U32)addr, val);
#   else
        _outp((DG_U16)(DG_U32)addr, (DG_S32)val);
#   endif
}

DG_DECL_PUBLIC
DG_U8
osl_par_hal_read8
(
        REG_SIZE* addr
)
{
    DG_U8 data;
#   if IO_LIB == OLS
        data = ReadIoPortByte((WORD)(DG_U32)addr);
#   else
        data = (DG_U8)_inp((DG_U16)(DG_U32)addr);
#   endif
    osl_debug_printf("Read: 0x%08X - 0x%04X\n",(DG_U32)addr,(DG_U8)data);
    return data;
}

DG_DECL_PUBLIC
DG_U32
osl_par_hal_read32
(
        REG_SIZE* addr
)
{
    DG_U32 data;
#   if IO_LIB == OLS
        data = ReadIoPortByte((WORD)(DG_U32)addr);
#   else
        data = (DG_U32)_inp((DG_U16)(DG_U32)addr);
#   endif
    osl_debug_printf("Read: 0x%08X - 0x%08X\n",(DG_U32)addr,(DG_U32)data);
    return data;
}

/* initializes the par driver (opens the par device) */
DG_DECL_PUBLIC
DG_RESULT
osl_par_hal_install
(
        DG_U32 *addr_list,
        DG_U8 addr_list_len,
        DG_U8 card_index
)
{
    DG_RESULT         result = DG_OK;
#   if IO_LIB == OLS
    DWORD           pciID;
#   elif IO_LIB == GIVEIO
    PCI_CONFIG      pci_cfg;
    PCI_UINT16      found;
#   endif


#if IO_LIB == OLS
    DWORD         ols_result = OLS_DLL_NO_ERROR;

    osl_debug_printf("OLS: Install starting\n");
    
    /* Initialization of the OpenLibSys driver to access IO space */
    if( FALSE == InitOpenLibSys(&hModule))
        result = DG_ERROR_OSL_INIT;
    else 
        ols_result = GetDllStatus();
    
    if (OLS_DLL_NO_ERROR != ols_result){
        result = DG_ERROR_OSL_INIT;
        switch (ols_result){
            case OLS_DLL_UNSUPPORTED_PLATFORM:
                osl_debug_printf("OLS: Platform not supported by DLL.\n");
                break;
            case OLS_DLL_DRIVER_NOT_LOADED:
                osl_debug_printf("OLS: Driver not loaded.\n");
                break;
            case OLS_DLL_DRIVER_NOT_FOUND:
                osl_debug_printf("OLS: Driver not found.\n");
                break;
            case OLS_DLL_DRIVER_UNLOADED:
                osl_debug_printf("OLS: Driver was unloaded by other process.\n");
                break;
            case OLS_DLL_DRIVER_NOT_LOADED_ON_NETWORK:
                osl_debug_printf("OLS: Driver not loaded because of executing on Network Drive (1.0.8 or later).\n");
                break;
            case OLS_DLL_UNKNOWN_ERROR:
                osl_debug_printf("OLS: Unknown error.\n");
                break;
            default:
                osl_debug_printf("OLS: Unknown error.\n");
        }

    }

    osl_debug_printf("OLS: Install finished\n");

#elif IO_LIB == GIVEIO
    OSVERSIONINFO   osvi;
    HANDLE          h;

    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    osl_debug_printf("Install starting\n");

    GetVersionEx(&osvi);

    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        h = CreateFile                  \
            (                           \
                "\\\\.\\giveio",        \
                GENERIC_READ,           \
                0,                      \
                NULL,                   \
                OPEN_EXISTING,          \
                FILE_ATTRIBUTE_NORMAL,  \
                NULL                    \
            );

        if (h == INVALID_HANDLE_VALUE)
        {
            result = DG_ERROR_OSL_INIT;
        } /* if (h == INVALID_HANDLE_VALUE) */

        CloseHandle(h);

    } /* if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) */
    osl_debug_printf("Install finished\n");
#endif



    /* If UE2G_HOST_BASE_CHANNEL_X is set to AUTODETECT perform a PCI scan */
    if (DG_HOST_BASE_CHANNEL_0(card_index) == AUTODETECT ) {
#       if IO_LIB == OLS
        pciID = FindPciDeviceById(0x1172,0xD221,0);
        if(0xFFFFFFFF == pciID)
        {
            osl_debug_printf ("No D221 device found\n");
            return DG_ERROR_OSL_INIT;
        }
#       elif IO_LIB == GIVEIO
        pci_cfg.vendor_id   = 0x1172;
        pci_cfg.device_id   = 0xD221;
        pci_cfg.bus         = 0x0;
        pci_cfg.dev         = 0x0;
        pci_cfg.func        = 0x0;

        osl_debug_printf ("Search D221 device...\n");
        pci_search(&pci_cfg,&found);

        if(FALSE == found)
        {
            osl_debug_printf ("No D221 device found\n");
            return DG_ERROR_OSL_INIT;
        }
#       endif
    }
    if ( DG_CARD_IO_MODE(card_index) == DG_HDCFG_PARALLEL && DG_HOST_BASE_CHANNEL_0(card_index) == AUTODETECT ) {
    
        int i;
        for (i=0;i<DG_NR_OF_CHANNELS;i++) {
            /* Store the base address of the channel in the array */
            /* Note, that D221 does only provide 1 io space with multiple channels */
#   if IO_LIB == OLS
            addr_list[i] = (DG_U32)((ReadPciConfigWord(pciID,16)&0xFFFFFFF8)+i*8);
#   elif IO_LIB == GIVEIO
            addr_list[i] = (DG_U32)((pci_cfg.base[0]&0xFFFFFFF8)+i*8);
#   endif
            break;
        }
    }
    else {
        DG_U8 i=0;
        DG_U32 j=0;
        if(i<addr_list_len)
            addr_list[i++] = DG_HOST_BASE_CHANNEL_0(card_index);
        if(i<addr_list_len)
            addr_list[i++] = DG_HOST_BASE_CHANNEL_1(card_index);
        if(i<addr_list_len)
            addr_list[i++] = DG_HOST_BASE_CHANNEL_2(card_index);
        if(i<addr_list_len)
            addr_list[i++] = DG_HOST_BASE_CHANNEL_3(card_index);
            
        for ( j=0+DG_HOST_BASE_CHANNEL_0(card_index); j<8+DG_HOST_BASE_CHANNEL_0(card_index); j++ )
        {
            osl_debug_printf("0x%08X: 0x%02X  ", j, osl_par_hal_read8((REG_SIZE*)j));
            osl_debug_printf("0x%08X: 0x%02X  ", j+8, osl_par_hal_read8((REG_SIZE*)j+8));
            osl_debug_printf("0x%08X: 0x%02X\n", j+16, osl_par_hal_read8((REG_SIZE*)j+16));
        } 
    }
    return result;
}

DG_DECL_PUBLIC
DG_RESULT
osl_par_hal_init_channel
(
        DG_HDREG_CONFIG* config
)
{
    DG_U8 fs; /* fifo_size */
    UE2G_REGISTER_MAP* base;
    UE1G_REGISTER_MAP* base_1g;
    UE3G_REGISTER_MAP* base_3g;
    DG_U8 clean_byte = 0x00;
    DG_U8   i;

    if (DG_HDCFG_CHANNEL_1G == ((DG_HDREG_CONFIG*)config)->dg_hdio_common_config.channel_type)
    {
        /* Synchronise access protocol of the MVB UART Emulation Procces Data*/
        /* transmits 34 data bytes with value 0x00, max. number of data bytes PD = 35*/
          base_1g = (UE1G_REGISTER_MAP*)((DG_HDREG_CONFIG*)config)->base;
         
          for(i = 0; i < 34; i++)
          {
              osl_par_hal_write8((REG_SIZE*)&base_1g->io,clean_byte);
          }
    
        ((DG_HDREG_CONFIG*)config) ->fifo_size = FIFO_SIZE(1);
    }
    else if (DG_HDCFG_CHANNEL_3G == ((DG_HDREG_CONFIG*)config)->dg_hdio_common_config.channel_type)
    {
        /* Synchronise access protocol of the MVB UART Emulation Message Data*/
        /* transmits 73 data bytes with value 0x00, max. number of data bytes MD = 74*/
        base_3g = (UE3G_REGISTER_MAP*)((DG_HDREG_CONFIG*)config)->base;
        
		for(i = 0; i < 73; i++)
        {
            osl_par_hal_write8((REG_SIZE*)&base_3g->io,clean_byte);
        }
    
        ((DG_HDREG_CONFIG*)config) ->fifo_size = FIFO_SIZE(1);
    }
    else
    {
        base = (UE2G_REGISTER_MAP*)((DG_HDREG_CONFIG*)config)->base;
        fs = osl_par_hal_read8((REG_SIZE*)&base->reg_fsr);
        ((DG_HDREG_CONFIG*)config) ->fifo_size = FIFO_SIZE(fs);
    }

    osl_debug_printf("fifoSize = %d \n", ((DG_HDREG_CONFIG*)config)->fifo_size);

    return DG_OK;
}


/*******************************************************************************
 * not implemented see other platforms
 *******************************************************************************/


DG_RESULT
dg_hdstm_init
(
  DG_U8 channel_num,
  DG_U8 card_index
)
{
    (void)channel_num;
    (void)card_index;
    return DG_ERROR_OSL_INIT;
}
DG_RESULT
dg_hdstm_install(DG_U8 card_index)
{
    (void)card_index;
    return DG_ERROR_OSL_INIT;
}
DG_RESULT
dg_hdstm_receive(
    DG_U8 channel_num,
    DG_U32* size,
    DG_U8* rxBuff,
    DG_U8 card_index
)
{
    (void) channel_num;
    (void) size;
    (void) rxBuff;
    (void) card_index;
    return DG_ERROR_OSL_READ;
}
DG_RESULT
dg_hdstm_transmit(
    DG_U8 channel_num,
    DG_U32* size,
    DG_U8* txBuff,
    DG_U8 card_index
)
{
    (void) channel_num;
    (void) size;
    (void) txBuff;
    (void) card_index;
    return DG_ERROR_OSL_WRITE;
}
DG_RESULT
osl_usb_hal_init_channel
(
  DG_U8 channel_num,
  DG_U8 card_index
)
{
    (void) channel_num;
    (void) card_index;
    return DG_ERROR_OSL_INIT;
}
DG_RESULT
osl_usb_hal_install(DG_U8 card_index)
{
    (void) card_index;
    return DG_ERROR_OSL_INIT;
}
DG_RESULT
osl_usb_hal_read(void)
{
    return DG_ERROR_OSL_READ;
}
DG_RESULT
osl_usb_hal_write(void)
{
    return DG_ERROR_OSL_WRITE;
}

/* --------------------------------------------------------------------------
 *  Procedure : pci_search
 * --------------------------------------------------------------------------
 */
extern
PCI_UINT16
pci_search
(
    PCI_CONFIG  *pci_cfg,
    PCI_UINT16  *found
)
{
    (void) pci_cfg;

    *found = FALSE;
    return(PCI_ERROR);

} /* pci_search */


#ifdef __cplusplus
}
#endif
