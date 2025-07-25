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



#include <windows.h>
#include <setupapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "os_def.h"
#include "dg_hdio.h"
#include "dg_hdreg.h"
#include "dg_reg1g.h"
#include "dg_reg2g.h"
#include "dg_reg3g.h"
#include "dg_error.h"
#include "public.h"

/* Global device data */
HANDLE  hDevice_0 = INVALID_HANDLE_VALUE;
HANDLE  hDevice_1 = INVALID_HANDLE_VALUE;
HANDLE  hDevice_2 = INVALID_HANDLE_VALUE;
HANDLE  hDevice_3 = INVALID_HANDLE_VALUE;

PSP_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetail;
HDEVINFO hDevInfo;

GUID guid_dpe_interface = {0x29d2a384, 0x2e47, 0x49b5, {0xae, 0xbf, 0x69, 0x62, 0xc2, 0x2b, 0xd7, 0xc2}};

DG_U32 byte_swap(DG_U32 in){
    return ((in & 0xff000000)>>24) | ((in & 0x00ff0000)>>8) | ((in & 0x0000ff00)<<8) | ((in & 0x000000ff)<<24);
}

void ReadPCI8( HANDLE fd, int offset, unsigned char *read8 )
{
    unsigned long bytes;
    struct pcipack pk;

    pk.revision = 1;
    pk.bar = 0;
    pk.offset = offset; /*odd address */
    pk.size = EIGHT;
    ReadFile(fd, &pk, sizeof(pk), &bytes, NULL);
    *read8 = pk.dataLOW;
}

void ReadPCI32( HANDLE fd, int offset, unsigned int *read32 )
{
    unsigned long bytes;
    struct pcipack pk;

    pk.revision = 1;
    pk.bar = 0;
    pk.offset = offset; /*odd address */
    pk.size = THIRTYTWO;
    ReadFile(fd, &pk, sizeof(pk), &bytes, NULL);
    *read32 = byte_swap(pk.dataLOW);
}

void WritePCI8( HANDLE fd, int offset, unsigned char write8 )
{
    unsigned long bytes;
    struct pcipack pk;

    pk.revision = 1;
    pk.bar = 0;
    pk.offset = offset; /*odd address */
    pk.size = EIGHT;
    pk.dataLOW = write8;
    WriteFile(fd, &pk, sizeof(pk), &bytes, NULL);
}

void WritePCI32( HANDLE fd, int offset, unsigned int write32 )
{
    unsigned long bytes;
    struct pcipack pk;

    pk.revision = 1;
    pk.bar = 0;
    pk.offset = offset; /*odd address */
    pk.size = THIRTYTWO;
    pk.dataLOW = byte_swap(write32);
    WriteFile(fd, &pk, sizeof(pk), &bytes, NULL);
}

/*
 * use a bit of Microsoft magic to find the device name, created on installation 
 */
 
int GetDevicePath(DG_U8 card_number)
{
    SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
    SP_DEVINFO_DATA DeviceInfoData;

    ULONG size;
    int count;
    BOOL status = TRUE;
    TCHAR *DeviceName = NULL;
    TCHAR *DeviceLocation = NULL;
    
    /* Get device information for all duagon PCI express (DPE) devices */
    hDevInfo = SetupDiGetClassDevs(&guid_dpe_interface, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);   
    /*  Initialize the SP_DEVICE_INTERFACE_DATA Structure. */
    DeviceInterfaceData.cbSize  = sizeof(SP_DEVICE_INTERFACE_DATA);
    /*  Determine how many devices are present. */
    count = 0;
    
    while(SetupDiEnumDeviceInterfaces(hDevInfo,  
                                      NULL,  
                                      &guid_dpe_interface,  
                                      count++,  /* Cycle through the available devices. */  
                                      &DeviceInterfaceData))
        ;

    /* Since the last call fails when all devices have been enumerated, */
    /* decrement the count to get the true device count. */
    count--;
    /*  If the count is zero then there are no devices present. */
    if (count == 0) 
    {
        osl_printf("No duagon PCI express (DPE) devices are present and enabled in the system.\n");
        return FALSE;
    }
    /*  Initialize the appropriate data structures in preparation for */
    /*  the SetupDi calls. */
    DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    /*  Loop through the device list to allow user to choose */
    /*  a device.  If there is only one device, select it */
    /*  by default. */
    if (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, (LPGUID)&guid_dpe_interface, card_number, &DeviceInterfaceData)) 
    {
        /* Determine the size required for the DeviceInterfaceData */
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &DeviceInterfaceData, NULL, 0, &size, NULL);
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
        {
            osl_printf("SetupDiGetDeviceInterfaceDetail failed, Error: %ld", GetLastError());
            return FALSE;
        }
        pDeviceInterfaceDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(size);
        if (!pDeviceInterfaceDetail) 
        {
            osl_printf("Insufficient memory.\n");
            return FALSE;
        }
        /* Initialize structure and retrieve data. */
        pDeviceInterfaceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        status = SetupDiGetDeviceInterfaceDetail(hDevInfo,
                                                 &DeviceInterfaceData,
                                                 pDeviceInterfaceDetail,
                                                 size,
                                                 NULL,
                                                 &DeviceInfoData);

        free(pDeviceInterfaceDetail);
        if (!status) 
        {
            osl_printf("SetupDiGetDeviceInterfaceDetail failed, Error: %ld", GetLastError());
            return status;
        }
        /*  Get the Device Name */
        /*  Calls to SetupDiGetDeviceRegistryProperty require two consecutive */
        /*  calls, first to get required buffer size and second to get the data. */
        SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &DeviceInfoData,
                                         SPDRP_DEVICEDESC,
                                         NULL,
                                         (PBYTE)DeviceName,
                                         0,
                                         &size);

        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
        {
            osl_printf("SetupDiGetDeviceRegistryProperty failed, Error: %ld", GetLastError());
            return FALSE;
        }
        DeviceName = (TCHAR*) malloc(size);
        if (!DeviceName) 
        {
            osl_printf("Insufficient memory.\n");
            return FALSE;
        }
        status = SetupDiGetDeviceRegistryProperty(hDevInfo,
                                                  &DeviceInfoData,
                                                  SPDRP_DEVICEDESC,
                                                  NULL,
                                                  (PBYTE)DeviceName,
                                                  size,
                                                  NULL);
        if (!status) 
        {
            osl_printf("SetupDiGetDeviceRegistryProperty failed, Error: %ld",
            GetLastError());
            free(DeviceName);
            return status;
        }
        /*  Get Device Location. */
        SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &DeviceInfoData,
                                         SPDRP_LOCATION_INFORMATION,
                                         NULL,
                                         (PBYTE)DeviceLocation,
                                         0,
                                         &size);

        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
        {
            DeviceLocation = (TCHAR*) malloc(size);
            if (DeviceLocation != NULL) 
            {
                status = SetupDiGetDeviceRegistryProperty(hDevInfo,  
                                                          &DeviceInfoData,  
                                                          SPDRP_LOCATION_INFORMATION,  
                                                          NULL,  
                                                          (PBYTE)DeviceLocation,  
                                                          size,  
                                                          NULL);  
                if (!status) 
                {
                    free(DeviceLocation);
                    DeviceLocation = NULL;
                }
            }
        } else 
        {
            DeviceLocation = NULL;
        }
        /* If there is more than one device print description. */
        osl_printf("%s\n", DeviceName);
        if (DeviceLocation) 
        {
            osl_printf("        %s\n", DeviceLocation);
        }
        free(DeviceName);
        DeviceName = NULL;
        if (DeviceLocation) 
        {
            free(DeviceLocation);
            DeviceLocation = NULL;
        }
    }
    /*  Get information for specific device. */
    status = SetupDiEnumDeviceInterfaces(hDevInfo,
                                         NULL,
                                         (LPGUID)&guid_dpe_interface,
                                         card_number,
                                         &DeviceInterfaceData);

    if (!status) 
    {
        osl_printf("SetupDiEnumDeviceInterfaces failed, Error: %ld \n", GetLastError());
        return status;
    }
    /* Determine the size required for the DeviceInterfaceData */
    SetupDiGetDeviceInterfaceDetail(hDevInfo,
                                    &DeviceInterfaceData,
                                    NULL,
                                    0,
                                    &size,
                                    NULL);

    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) 
    {
        osl_printf("SetupDiGetDeviceInterfaceDetail failed, Error: %ld \n", GetLastError());
        return FALSE;
    }
    pDeviceInterfaceDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc(size);
    if (!pDeviceInterfaceDetail) 
    {
        osl_printf("Insufficient memory.\n");
        return FALSE;
    }
    /* Initialize structure and retrieve data. */
    pDeviceInterfaceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    status = SetupDiGetDeviceInterfaceDetail(hDevInfo,
                                             &DeviceInterfaceData,
                                             pDeviceInterfaceDetail,
                                             size,
                                             NULL,
                                             &DeviceInfoData);
    if (!status) 
    {
        osl_printf("SetupDiGetDeviceInterfaceDetail failed, Error: %ld \n", GetLastError());
        return status;
    }
    return status;
}

/* 
 * Host / Device interface 
 */

DG_DECL_PUBLIC
void
osl_par_hal_write8
(
        REG_SIZE* addr,
        DG_U8 val
)
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    int address = (unsigned int)addr & 0xFFFF0000;

    /* use high word of offset to select the correct file descriptor this card */
    if(address == 0x000000) hDevice = hDevice_0;
    if(address == 0x010000) hDevice = hDevice_1;
    if(address == 0x020000) hDevice = hDevice_2;
    if(address == 0x030000) hDevice = hDevice_3;
    addr = (REG_SIZE*)((unsigned int)addr & 0x0000FFFF);
    WritePCI8(hDevice, (int)addr, val);
}

DG_DECL_PUBLIC
void
osl_par_hal_write32
(
        REG_SIZE* addr,
        DG_U32 val
)
{
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    int address = (unsigned int)addr & 0xFFFF0000;

    /* use high word of offset to select the correct file descriptor this card */
    if(address == 0x000000) hDevice = hDevice_0;
    if(address == 0x010000) hDevice = hDevice_1;
    if(address == 0x020000) hDevice = hDevice_2;
    if(address == 0x030000) hDevice = hDevice_3;
    addr = (REG_SIZE*)((unsigned int)addr & 0x0000FFFF);
    WritePCI32(hDevice, (int)addr, val);
}

DG_DECL_PUBLIC
DG_U8
osl_par_hal_read8
(
        REG_SIZE* addr
)
{
    DG_U8 data;
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    int address = (unsigned int)addr & 0xFFFF0000;

    /* use high word of offset to select the correct file descriptor this card */
    if(address == 0x000000) hDevice = hDevice_0;
    if(address == 0x010000) hDevice = hDevice_1;
    if(address == 0x020000) hDevice = hDevice_2;
    if(address == 0x030000) hDevice = hDevice_3;
    addr = (REG_SIZE*)((unsigned int)addr & 0x0000FFFF);
    ReadPCI8(hDevice, (int)addr, &data);
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
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    int address = (unsigned int)addr & 0xFFFF0000;

    /* use high word of offset to select the correct file descriptor this card */
    if(address == 0x000000) hDevice = hDevice_0;
    if(address == 0x010000) hDevice = hDevice_1;
    if(address == 0x020000) hDevice = hDevice_2;
    if(address == 0x030000) hDevice = hDevice_3;
    addr = (REG_SIZE*)((unsigned int)addr & 0x0000FFFF);
    ReadPCI32(hDevice, (int)addr, &data);
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
    int i;
    BOOL status;
    DG_U8 cd_nbr;
	
    DG_U8 card_number = DG_NO_CARD;
    status = FALSE;
    for ( cd_nbr=0; cd_nbr<DG_MAX_CARDS_PER_DRIVER; cd_nbr++)
    {
       if ( card_defined[cd_nbr].defined && card_defined[cd_nbr].index == card_index )
           card_number = cd_nbr;
    } 
    
    switch (card_number)
    {
    int x;
    case 0:
        for ( x=0; x<addr_list_len; x++ )
        {
            addr_list[x] = x*8;
        }
        if(hDevice_0 == INVALID_HANDLE_VALUE)
        {
            status = GetDevicePath(card_number);
            osl_printf ( "card_number(%d)\n", card_number );
            if (status == TRUE)
            {
                osl_printf ( "CreateFile(%s)\n", pDeviceInterfaceDetail->DevicePath );
                hDevice_0 = CreateFile(pDeviceInterfaceDetail->DevicePath,
                                GENERIC_READ|GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);
            }
            if (hDevice_0 == INVALID_HANDLE_VALUE) 
            {
                osl_printf("can't access PCIe. is module installed? [open returned 0x%08lx]\n", GetLastError());
                return DG_ERROR_OSL_INIT;
            }
        }
        for ( i=0; i<8; i++ )
        {
            for ( x=0; x<addr_list_len; x++ )
            {
                osl_printf("0x%08X: 0x%02X  ", 8*x + i, osl_par_hal_read8((REG_SIZE*)(8*x + i)));
            }
            osl_printf("\n");
        }
        break;
    case 1:
        for ( x=0; x<addr_list_len; x++ )
        {
            /* use high word of offset to indicate the correct file descriptor this card */
            addr_list[x] = x*8 + 0x010000;
        }
        if(hDevice_1 == INVALID_HANDLE_VALUE)
        {
            osl_printf ( "card_number(%d)\n", card_number );
            status = GetDevicePath(card_number);
            if (status == TRUE)
            {
                osl_printf ( "CreateFile(%s)\n", pDeviceInterfaceDetail->DevicePath );
                hDevice_1 = CreateFile(pDeviceInterfaceDetail->DevicePath,
                                GENERIC_READ|GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);
            }
            if (hDevice_1 == INVALID_HANDLE_VALUE) 
            {
                osl_printf("can't access PCIe. is module installed? [open returned 0x%08lx]\n", GetLastError());
                return DG_ERROR_OSL_INIT;
            }
        }
        for ( i=0; i<8; i++ )
        {
            for ( x=0; x<addr_list_len; x++ )
            {
                osl_printf("0x%08X: 0x%02X  ", 8*x + i, osl_par_hal_read8((REG_SIZE*)(8*x + i +0x010000)));
            }
            osl_printf("\n");
        }
        break;
    case 2:
        for ( x=0; x<addr_list_len; x++ )
        {
            /* use high word of offset to indicate the correct file descriptor this card */
            addr_list[x] = x*8 + 0x020000;
        }
        if(hDevice_2 == INVALID_HANDLE_VALUE)
        {
            status = GetDevicePath(card_number);
            osl_printf ( "card_number(%d)\n", card_number );
            if (status == TRUE)
            {
                osl_printf ( "CreateFile(%s)\n", pDeviceInterfaceDetail->DevicePath );
                hDevice_2 = CreateFile(pDeviceInterfaceDetail->DevicePath,
                                GENERIC_READ|GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);
            }
            if (hDevice_2 == INVALID_HANDLE_VALUE) 
            {
                osl_printf("can't access PCIe. is module installed? [open returned 0x%08lx]\n", GetLastError());
                return DG_ERROR_OSL_INIT;
            }
        }
        for ( i=0; i<8; i++ )
        {
            for ( x=0; x<addr_list_len; x++ )
            {
                osl_printf("0x%08X: 0x%02X  ", 8*x + i, osl_par_hal_read8((REG_SIZE*)(8*x + i +0x020000)));
            }
            osl_printf("\n");
        }
        break;
    case 3:
        for ( x=0; x<addr_list_len; x++ )
        {
            /* use high word of offset to indicate the correct file descriptor this card */
            addr_list[x] = x*8 + 0x030000;
        }
         if(hDevice_3 == INVALID_HANDLE_VALUE)
        {
            status = GetDevicePath(card_number);
            osl_printf ( "card_number(%d)\n", card_number );
            if (status == TRUE)
            {
                osl_printf ( "CreateFile(%s)\n", pDeviceInterfaceDetail->DevicePath );
                hDevice_3 = CreateFile(pDeviceInterfaceDetail->DevicePath,
                                GENERIC_READ|GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);
            }
            if (hDevice_3 == INVALID_HANDLE_VALUE) 
            {
                osl_printf("can't access PCIe. is module installed? [open returned 0x%08lx]\n", GetLastError());
                return DG_ERROR_OSL_INIT;
            }
        }
        for ( i=0; i<8; i++ )
        {
            for ( x=0; x<addr_list_len; x++ )
            {
                osl_printf("0x%08X: 0x%02X  ", 8*x + i, osl_par_hal_read8((REG_SIZE*)(8*x + i +0x030000)));
            }
            osl_printf("\n");
        }
        break;
    default:
        return DG_ERROR_OSL_INIT;
        break;
    }
    return DG_OK;
    
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
    return DG_OK;
}


