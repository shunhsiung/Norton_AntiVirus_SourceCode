// Copyright 1998 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVAPI/VCS/boot.cpv   1.16   13 Oct 1998 21:05:32   MKEATIN  $
//
// Description: Contains NAVAPI's boot record API's
//
//************************************************************************
// $Log:   S:/NAVAPI/VCS/boot.cpv  $
//
//    Rev 1.16   13 Oct 1998 21:05:32   MKEATIN
// On Win16, NAVScanBoot now always scans the active partition on drive
// 0x80 - no matter what drive letter is passed.  This is because
// DiskMapLogToPhyParams chunks on Windows 3.x and we don't want to
// invest in fixing it.
//
//    Rev 1.15   21 Aug 1998 12:27:52   MKEATIN
// Now mapping floppies without calling DiskMapLogToPhyParams which was causing
// unstable systems on Win16.
//
//    Rev 1.14   19 Aug 1998 21:18:56   MKEATIN
// Did some work on MapDriveLetter.  DiskGetPhysicalInfo fails on Win16 floppy
// disks.  So, byDriveNumber = cDriveLetter - 'A' and byPartEntry is 0.
//
//    Rev 1.13   30 Jul 1998 21:51:20   DHERTEL
// Changes for NLM packaging of NAVAPI.
//
//    Rev 1.12   29 Jul 1998 18:33:34   DHERTEL
// DX and NLM changes
//
//    Rev 1.11   22 Jul 1998 14:14:32   MKEATIN
// Enable/disable auto-protect on WIN32 only.
//
//    Rev 1.10   23 Jun 1998 15:25:08   DALLEE
// Updated error return codes.
// Now support mem, access, no repair, and invalid arg returns.
//
//    Rev 1.9   22 Jun 1998 22:28:16   DALLEE
// Added calls to enable/disable NAV auto-protect around scanning.
//
//    Rev 1.8   22 Jun 1998 14:47:48   MKEATIN
// We now make sure NULL is not passed as our lphVirus in boot scanning.
//
//    Rev 1.7   19 Jun 1998 17:28:04   DALLEE
// Removed cookie parameter - no callbacks called from boot scanning.
//
//    Rev 1.6   17 Jun 1998 17:48:38   MKEATIN
// Now, returning NAV_xxx error codes and check for valid arguments -
// basically valid hEngine's that is.
//
//    Rev 1.5   28 May 1998 17:03:10   MKEATIN
// Uppercase cDriveletter if necessary on NAVRepairBoot.
//
//    Rev 1.4   28 May 1998 12:59:48   MKEATIN
// Added NAVRepairBoot and NAVRepairMasterBoot
//
//    Rev 1.3   27 May 1998 19:38:50   MKEATIN
// Changed code to match our new naming conventions
//
//    Rev 1.2   27 May 1998 16:26:14   MKEATIN
//************************************************************************

#include "platform.h"
#include "xapi.h"
#if !defined(SYM_UNIX)
#include "disk.h"
#endif
#include "ctype.h"
#include "avapi_l.h"
#include "navcb_l.h"
#include "nlm_nav.h"
#include "navapi.h"
#include "apenable.h"

// local function prototypes

STATUS LOCAL MapDriveLetter
(
    BYTE   byDriveLetter,
    LPBYTE lpbyDriveNumber,
    LPBYTE lpbyPartEntry
);

#ifdef SYM_WIN32
  #include <winioctl.h>

  // KLUGE ALERT: PARTITION_INFORMATION structure is 28 bytes long, but
  // depending on structure packing changes to 32 bytes.  WinNT expects
  // 32 to be passed in as the size to DeviceIoControl().

  typedef union tagBYTE_PARTITION_INFORMATION_32BYTE {
    PARTITION_INFORMATION pi;
    BYTE abyFiller [32];
    } PARTITION_INFORMATION_32BYTE;
#endif

#if !defined(SYM_NLM) && !defined(SYM_UNIX)
UINT WINAPI NAVEXSDiskReadPhysical
(
    LPVOID  lpBuffer,
    BYTE    bCount,
    WORD    wCylinder,
    BYTE    bSector,
    BYTE    bHead,
    BYTE    bDrive
);
#endif


//************************************************************************
// NAVSTATUS NAVCALLAPI NAVScanBoot
// (
//     HNAVENGINE       hNAVEngine,   // [in] a valid NAV engine handle
//     char             cDriveLetter, // [in] logical drive letter (A, C, etc.)
//     HNAVVIRUS*       lphVirus      // [out] valid HNAVVIRUS if infected
// )
//
// This routine scans for viruses in the Boot Record of cDriveLetter. If a
// virus is found lphVirus will reference a virus handle.  If no virus is
// found lphVirus will reference a NULL pointer.
//
// Returns:
//      NAV_OK on success.
//
//      On error, one of:
//          NAV_MEMORY_ERROR
//          NAV_INVALID_ARG
//          NAV_NO_ACCESS
//          NAV_ERROR
//************************************************************************
// 05/27/98 Created by MKEATIN
//************************************************************************

NAVSTATUS NAVCALLAPI NAVScanBoot
(
    HNAVENGINE       hNAVEngine,   // [in] a valid NAV engine handle
    char             cDriveLetter, // [in] logical drive letter (A, C, etc.)
    HNAVVIRUS FAR *  lphVirus      // [out] valid HNAVVIRUS if infected
)
{
#if defined(SYM_NLM) || defined(SYM_UNIX)

    return NAV_ERROR;

#else

    HVLVIRUS    hVirus;
    BYTE        byDriveNumber, byPartEntry;
    LPBYTE      lpBootBuffer = NULL;
    DISKREC     disk         = {0};
    VSTATUS     vStatus;
    NAVSTATUS   nStatus      = NAV_OK;

    SYM_ASSERT( hNAVEngine );

    // Make sure lphVirus is not NULL

    if ( !lphVirus )
        return NAV_INVALID_ARG;

    // Disable NAVAP for this process

#ifdef SYM_WIN32
    NavapUnprotectProcess();
#endif

    // initialize the virus handle to NULL

    *lphVirus = NULL;

    // uppercase cDriveLetter

    cDriveLetter = CharIsUpper(cDriveLetter) ? cDriveLetter : CharToUpper(cDriveLetter);

    if (MapDriveLetter(cDriveLetter,
                       &byDriveNumber,
                       &byPartEntry) == ERR)
    {

        // if we fail to get a physical drive and partition number,  we'll attempt
        // this logical read.  NavEx will not be used nor will viruses be repairable here

        lpBootBuffer = (LPBYTE)MemAllocPtr(GHND, PHYSICAL_SECTOR_SIZE * 8);

        if (!lpBootBuffer)
        {
            nStatus = NAV_MEMORY_ERROR;
            goto BailOut;
        }

        // setup a signature in our buffer

        STRCPY((LPSTR)lpBootBuffer, "SYMC");

        if (FALSE == DiskGetInfo(cDriveLetter, &disk))
        {
            nStatus = NAV_NO_ACCESS;
            goto BailOut;
        }

        disk.dl = (BYTE) cDriveLetter;
        disk.sector = 0;
        disk.sectorCount = 1;
        disk.buffer = lpBootBuffer;

        if (DiskBlockDevice(BLOCK_READ, &disk))
        {
            nStatus = NAV_NO_ACCESS;
            goto BailOut;
        }

        // Did will really read our boot sector? SYMC should be overwritten if so.

        if (!STRNCMP((LPSTR)lpBootBuffer, "SYMC", 4))
        {
            nStatus = NAV_NO_ACCESS;
            goto BailOut;
        }
    }

    vStatus = VLScanBoot((HVCONTEXT)hNAVEngine,
                         byDriveNumber,
                         byPartEntry,
                         lpBootBuffer,
                         &hVirus);

    // Translate VSTATUS to NAVSTATUS result

    switch ( vStatus )
    {
    case VS_OK:
        *lphVirus = hVirus;
        nStatus = NAV_OK;
        break;

    case VS_MEMALLOC:
        nStatus = NAV_MEMORY_ERROR;
        break;

    case VS_INVALID_ARG:
        nStatus = NAV_INVALID_ARG;
        break;

    case VS_NO_ACCESS:
        nStatus = NAV_NO_ACCESS;
        break;

    default:
        nStatus = NAV_ERROR;
        break;
    }

BailOut:
    if (lpBootBuffer)
        MemFreePtr(lpBootBuffer);

#ifdef SYM_WIN32
    NavapProtectProcess();
#endif

    return ( nStatus );

#endif  // #if defined(SYM_NLM) || defined(SYM_UNIX) #else
}


//************************************************************************
// NAVSTATUS NAVCALLAPI NAVScanMasterBoot
// (
//     HNAVENGINE         hNAVEngine,   // [in] a valid NAV engine handle
//     UINT               uPhysDriveNum,// [in] physical drive number (0x80, 0x81, etc.)
//     HNAVVIRUS*         lphVirus      // [in/out] HNAVVIRUS* allocate in or NULL
// )
//
// This routine scans for viruses in the Master Boot Record. If a virus is
// found lphVirus will reference a virus handle.  If no virus is found
// lphVirus will reference a NULL pointer.
//
// Returns:
//      NAV_OK on success.
//
//      On error, one of:
//          NAV_MEMORY_ERROR
//          NAV_INVALID_ARG
//          NAV_NO_ACCESS
//          NAV_ERROR
//************************************************************************
// 05/27/98 Created by MKEATIN
//************************************************************************

NAVSTATUS NAVCALLAPI NAVScanMasterBoot
(
    HNAVENGINE         hNAVEngine,   // [in] a valid NAV engine handle
    UINT               uPhysDriveNum,// [in] physical drive number (0x80, 0x81, etc.)
    HNAVVIRUS FAR *    lphVirus      // [in/out] HNAVVIRUS* allocate in or NULL
)
{
#if defined(SYM_NLM) || defined(SYM_UNIX)

    return NAV_ERROR;

#else

    HVLVIRUS    hVirus  = NULL;
    VSTATUS     vStatus;

    SYM_ASSERT( hNAVEngine );

    // Make sure lphVirus is not NULL

    if ( !lphVirus )
        return NAV_INVALID_ARG;

    // initialize the virus handle to NULL

    *lphVirus = NULL;

#ifdef SYM_WIN32
    NavapUnprotectProcess();
#endif

    vStatus = VLScanPart((HVCONTEXT)hNAVEngine,
                                uPhysDriveNum,
                                NULL,
                                &hVirus);

#ifdef SYM_WIN32
    NavapProtectProcess();
#endif

    // Check to see if a virus was found

    switch ( vStatus )
    {
    case VS_OK:
        *lphVirus = hVirus;
        return NAV_OK;

    case VS_MEMALLOC:
        return NAV_MEMORY_ERROR;

    case VS_INVALID_ARG:
        return NAV_INVALID_ARG;

    case VS_NO_ACCESS:
        return NAV_NO_ACCESS;

    default:
        return NAV_ERROR;
    }

#endif  // #if defined(SYM_NLM) || defined(SYM_UNIX) #else
}


//************************************************************************
// NAVSTATUS NAVCALLAPI NAVRepairBoot
// (
//     HNAVENGINE       hNAVEngine,   // [in] a valid NAV engine handle
//     char             cDriveLetter  // [in] logical drive letter (A, C, etc.)
// )
//
// This routine repairs viruses found in the logical drive cDriveLetter.
//
// Returns:
//      NAV_OK on success.
//
//      On error, one of:
//          NAV_MEMORY_ERROR
//          NAV_INVALID_ARG
//          NAV_NO_ACCESS
//          NAV_CANT_REPAIR
//          NAV_ERROR
//************************************************************************
// 05/28/98 Created by MKEATIN
//************************************************************************

NAVSTATUS NAVCALLAPI NAVRepairBoot
(
    HNAVENGINE       hNAVEngine,   // [in] a valid NAV engine handle
    char             cDriveLetter  // [in] logical drive letter (A, C, etc.)
)
{
#if defined(SYM_NLM) || defined(SYM_UNIX)

    return NAV_ERROR;

#else

    HVLVIRUS    hVirus;
    BYTE        byDriveNumber, byPartEntry;
    VSTATUS     vStatus;
    NAVSTATUS   nStatus;

    SYM_ASSERT( hNAVEngine );

    // Disable NAVAP for this process

#ifdef SYM_WIN32
    NavapUnprotectProcess();
#endif

    // uppercase cDriveLetter

    cDriveLetter = CharIsUpper(cDriveLetter) ? cDriveLetter : CharToUpper(cDriveLetter);

    if ( ERR == MapDriveLetter( cDriveLetter,
                                &byDriveNumber,
                                &byPartEntry ) )
    {
        nStatus = NAV_CANT_REPAIR;
    }
    else
    {
        // Rescan to get new HVLVIRUS

        vStatus = VLScanBoot(hNAVEngine,
                            byDriveNumber,
                            byPartEntry,
                            NULL,
                            &hVirus);

        if ( VS_OK == vStatus )
        {
            if ( NULL != hVirus )
            {
                // Repair infection.

                vStatus = VLRepairItem( hVirus, NULL );

                VLReleaseVirusHandle( hVirus );
            }
            else
            {
                // Item was not infected...

                vStatus = VS_CANT_REPAIR;
            }
        }

        // Translate VSTATUS to NAVSTATUS result.

        switch ( vStatus )
        {
        case VS_OK:
            nStatus = NAV_OK;
            break;

        case VS_MEMALLOC:
            nStatus = NAV_MEMORY_ERROR;
            break;

        case VS_INVALID_ARG:
            nStatus = NAV_INVALID_ARG;
            break;

        case VS_NO_ACCESS:
            nStatus = NAV_NO_ACCESS;
            break;

        case VS_CANT_REPAIR:
            nStatus = NAV_CANT_REPAIR;
            break;

        default:
            nStatus = NAV_ERROR;
            break;
        }
    }

    // Reenable Auto-Protect

#ifdef SYM_WIN32
    NavapProtectProcess();
#endif

    return ( nStatus );

#endif  // #if defined(SYM_NLM) || defined(SYM_UNIX) #else
}


//************************************************************************
// NAVSTATUS NAVCALLAPI NAVRepairMasterBoot
// (
//     HNAVENGINE       hNAVEngine,   // [in] a valid NAV engine handle
//     UINT             uPhysDriveNum // [in] physical drive number (0x80, 0x81, etc.)
// )
//
// This routine repairs viruses found in the Master Boot Record of
// the physical drive uPhysDriveNum.
//
// Returns:
//      NAVSTATUS error code
//************************************************************************
// 05/28/98 Created by MKEATIN
//************************************************************************

NAVSTATUS NAVCALLAPI NAVRepairMasterBoot
(
    HNAVENGINE       hNAVEngine,   // [in] a valid NAV engine handle
    UINT             uPhysDriveNum // [in] physical drive number (0x80, 0x81, etc.)
)
{
#if defined(SYM_NLM) || defined(SYM_UNIX)

    return NAV_ERROR;

#else

    HVLVIRUS    hVirus;
    VSTATUS     vStatus;
    NAVSTATUS   nStatus;

    SYM_ASSERT( hNAVEngine );

    // Disable NAVAP for this process

#ifdef SYM_WIN32
    NavapUnprotectProcess();
#endif

    // Get virus handle.

    vStatus = VLScanPart(hNAVEngine,
                         uPhysDriveNum,
                         NULL,
                         &hVirus);

    if ( VS_OK == vStatus )
    {
        if ( NULL != hVirus )
        {
            // Repair infection.

            vStatus = VLRepairItem( hVirus, NULL );

            VLReleaseVirusHandle( hVirus );
        }
        else
        {
            // Item was not infected...

            vStatus = VS_CANT_REPAIR;
        }
    }

    // Translate VSTATUS to NAVSTATUS result.

    switch ( vStatus )
    {
    case VS_OK:
        nStatus = NAV_OK;
        break;

    case VS_MEMALLOC:
        nStatus = NAV_MEMORY_ERROR;
        break;

    case VS_INVALID_ARG:
        nStatus = NAV_INVALID_ARG;
        break;

    case VS_NO_ACCESS:
        nStatus = NAV_NO_ACCESS;
        break;

    case VS_CANT_REPAIR:
        nStatus = NAV_CANT_REPAIR;
        break;

    default:
        nStatus = NAV_ERROR;
        break;
    }

    // Reenable Auto-Protect

#ifdef SYM_WIN32
    NavapProtectProcess();
#endif

    return ( nStatus );

#endif  // #if defined(SYM_NLM) || defined(SYM_UNIX) #else
}


//************************************************************************
// STATUS LOCAL MapDriveLetter
// (
//     BYTE   byDriveLetter,               // [in]  Drive letter to map
//     LPBYTE lpbyDriveNumber,             // [out] Drive number
//     LPBYTE lpbyPartEntry                // [out] Partition Entry
// )
//
// This routine maps a logical drive letter to its physical drive number
// and partition entry.
//
// Returns:
//      NOERR if succussful, otherwise ERR
//************************************************************************
// 04/24/97 Created by MKEATIN
// 05/24/99 Ported Win2000 boot record changes from
//          VirScan.cpp:VirusScanMapDriveLetter.
//************************************************************************

#if !defined(SYM_NLM) && !defined(SYM_UNIX)

STATUS LOCAL MapDriveLetter
(
    BYTE   byDriveLetter,               // [in]  Drive letter to map
    LPBYTE lpbyDriveNumber,             // [out] Drive number
    LPBYTE lpbyPartEntry                // [out] Partition Entry
)
{
    ABSDISKREC     stAbsDisk = {0};
    INT13REC       stInt13Rec;
    BYTE           byPartEntry;
    DWORD          dwLinearBootSector;

                                        // Initialize memory.
    memset (&stInt13Rec, 0, sizeof (stInt13Rec));


    if (HWIsNEC() && !DiskIsFixed(byDriveLetter)) // we do a simple mappying on NEC floppies
    {                                             // here.
        *lpbyDriveNumber = byDriveLetter - 'A';
        *lpbyPartEntry   = 0;
        return (NOERR);
    }
    else if (HWIsNEC())
    {                                   // no hard drive boot scanning on NEC
        return (ERR);
    }

#ifdef SYM_WIN16
    if (!DiskIsFixed(byDriveLetter))    // we do a simple mappying on floppies
    {                                   // here.
        *lpbyDriveNumber = byDriveLetter - 'A';
        *lpbyPartEntry   = 0;
        return (NOERR);
    }

    byDriveLetter = 'C';                // we're forcing the boot scanning to the active partition
#endif                                  // on drive 0x80

    if (byDriveLetter == 'C')
    {
        stAbsDisk.dn = 0x80;

        DiskGetPhysicalInfo(&stAbsDisk);

        stAbsDisk.dwHead     = 0;
        stAbsDisk.dwTrack    = 0;
        stAbsDisk.dwSector   = 1;
        stAbsDisk.numSectors = 1;
        stAbsDisk.buffer = (HPBYTE)MemAllocPtr(GHND, 8 * PHYSICAL_SECTOR_SIZE);

        if (!stAbsDisk.buffer)
        {
            return (ERR);
        }

        if (!DiskAbsOperation(READ_COMMAND, &stAbsDisk))
        {
            for (byPartEntry = 0; byPartEntry < 4; byPartEntry++)
            {
                LPPARTENTRYREC lpPartEntryRec =
                    (LPPARTENTRYREC)(stAbsDisk.buffer + PART_TABLE_OFFSET) +
                    byPartEntry;

                                            // Is this an unused partition entry?

                if (lpPartEntryRec->system == SYSTEM_NOT_USED)
                    continue;
                                            // for drive 'C' we scan the
                                            // active partition only

                if (lpPartEntryRec->bootable)
                {
                    *lpbyDriveNumber = stAbsDisk.dn;
                    *lpbyPartEntry   = byPartEntry;

                    MemFreePtr(stAbsDisk.buffer);
                    return (NOERR);
                }
            }
        }
        MemFreePtr(stAbsDisk.buffer);

#ifdef SYM_WIN16                        // only scan the active partion of drive 0x80
        return (ERR);                   // on Win16
#endif
    }
                                        // Okay, this is not drive 'C'  - or
                                        // it is drive 'C' but there is no
                                        // active partition on drive 0x80

#ifdef SYM_WIN32

    if (SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
    {
        OSVERSIONINFO   OSVersionInfo;
        BOOL            bOsVersionInfoValid;

        ZeroMemory(&OSVersionInfo, sizeof(OSVersionInfo));
        OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVersionInfo);

        bOsVersionInfoValid = GetVersionEx((LPOSVERSIONINFO)&OSVersionInfo);
        if (TRUE == bOsVersionInfoValid)
        {
            if (VER_PLATFORM_WIN32_NT == OSVersionInfo.dwPlatformId)
            {

                if (OSVersionInfo.dwMajorVersion >= 5) //for Windows 2000 and beyond
                {

                    static  char                 szFloppy [] = "\\Device\\Floppy";
                    char                         szPartition[SYM_MAX_PATH]= "\\\\.\\";
                    char                         szDevice[SYM_MAX_PATH];
                    char                         szDrive[3] = "?:";

                    HANDLE                       DeviceHandle;
                    DWORD                        dwBytesReturned;
                    STORAGE_DEVICE_NUMBER        StorageDeviceInfo;
                    PARTITION_INFORMATION_32BYTE PartitionInfo;
                    DISK_GEOMETRY                DiskGeometryInfo;

                    DWORD                        dwSectorOffset;
                    DWORD                        dwTrackOffset;
                    DWORD                        dwDeviceQueryStatus;
                    DWORD                        dwDrive;


                    szDrive[0] = byDriveLetter;
                    STRCAT(szPartition, szDrive);

                    //
                    // The following logic handles the case where the
                    // floppy drive needs to be handled under Windows 2000.
                    //
                    dwDeviceQueryStatus = QueryDosDevice(szDrive, szDevice, sizeof(szDevice));

                    // WJORDAN 11-05-2001: Changed the logic so that the handle to the device
                    //                     is not obtained for floppy drives. The handle is only
                    //                     used for non floppy devices and obtaining it causes
                    //                     Defect# 374088. When Disk Copy is attempting to write
                    //                     the newly mounted floppy disk we obtain this handle and
                    //                     Disk Copy fails with an error message.
                    if ( (0 != dwDeviceQueryStatus                           ) &&
                         (0 == STRNICMP(szDevice, szFloppy, STRLEN(szFloppy)))
                       )
                    {
                        ConvertStringToLong(&dwDrive, szDevice + STRLEN(szFloppy));

                        stInt13Rec.generated = TRUE;
                        stInt13Rec.dwTrack   = 0;
                        stInt13Rec.dwHead    = 0;
                        stInt13Rec.dwSector  = 1;
                        stInt13Rec.dn        = (BYTE)dwDrive;
                    }//end if (0 != dwDeviceQueryStatus && (0==STRNICMP...)

                    // WJORDAN 11-05-2001: This block will handle all of the non floppy devices
                    else
                    {
                        DeviceHandle =
                            CreateFile(szPartition,
                                       GENERIC_READ,
                                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                                       NULL,
                                       OPEN_EXISTING,
                                       FILE_ATTRIBUTE_NORMAL,
                                       NULL
                                      );
						//
						// The following logic handles the case where the
						// DeviceHandle is valid and the device information
						// needs to be obtained for basic disks under
						// Windows 2000.
						//

						if ( ( INVALID_HANDLE_VALUE != DeviceHandle ) &&
							 ( TRUE == DeviceIoControl(DeviceHandle,
										 IOCTL_STORAGE_GET_DEVICE_NUMBER,
										 NULL, 0,
										 &StorageDeviceInfo, sizeof(StorageDeviceInfo),
										 &dwBytesReturned, NULL)) &&
							 ( TRUE == DeviceIoControl(DeviceHandle,
										 IOCTL_DISK_GET_PARTITION_INFO,
										 NULL, 0,
										 &PartitionInfo, sizeof(PartitionInfo),
										 &dwBytesReturned, NULL)) &&
							 ( TRUE == DeviceIoControl(DeviceHandle,
										 IOCTL_DISK_GET_DRIVE_GEOMETRY,
										 NULL, 0,
										 &DiskGeometryInfo, sizeof(DiskGeometryInfo),
										 &dwBytesReturned, NULL)) )
						{

							dwSectorOffset = PartitionInfo.pi.StartingOffset.LowPart
											 / DiskGeometryInfo.BytesPerSector;

							dwTrackOffset = dwSectorOffset
											/ DiskGeometryInfo.SectorsPerTrack;

											// Compute location of sector 0.
							stInt13Rec.generated = TRUE;

							stInt13Rec.dwTrack =
								(WORD)(dwTrackOffset / DiskGeometryInfo.TracksPerCylinder);

							stInt13Rec.dwHead =
								(BYTE)(dwTrackOffset % DiskGeometryInfo.TracksPerCylinder);

							stInt13Rec.dwSector =
								1 + (BYTE)(dwSectorOffset % DiskGeometryInfo.SectorsPerTrack);

							stInt13Rec.dn = (BYTE)(StorageDeviceInfo.DeviceNumber + FIRST_HD);

						}//end if

						CloseHandle(DeviceHandle);
                    } // end else Non-floppy device case

                }//end if (OSVersionInfo.dwMajorVersion >= 5)



                else//do the "Windows NT 4.0 and before" thing
                {
                                // MMENDON 04-10-2001 Floppy access change STS inc.415067
                                //                    If FASTFAT.SYS was not loaded, and
                                //                    a floppy was accessed, the call to
                                //                    CreateFile() was causing the Raw FS
                                //                    to be mounted on the floppy before
                                //                    FAT could be mounted. The accessing
                                //                    thread (i.e. explorer) would then 
                                //                    pop up an error message because it
                                //                    could not use the Raw FS.
                                //                    Change:  The CreateFile() call is not
                                //                    needed for floppies and was moved inside 
                                //                    the Hard Disk block.  To prevent the Raw FS 
                                //                    mount on subsequent CreateFile() calls on this
                                //                    thread, we are forcing FAT to load and mount
                                //                    by first calling GetVolumeInformation()
                                //                    when the device is a floppy.

                    static  char    szHard   [] = "\\Device\\Harddisk";
                    static  char    szFloppy [] = "\\Device\\Floppy";

                    char    szDrive [3] = "?:";
                    HANDLE  hDrive;
                    HANDLE  hPhysDrive;

                    char    szPhysDrive [] = "\\\\.\\PHYSICALDRIVEx";
                    DWORD   dwDrive;
                    DWORD   dwSectorOffset;
                    DWORD   dwTrackOffset;

                    PARTITION_INFORMATION_32BYTE    rPartInfo;
                    DISK_GEOMETRY                   rDiskGeometry;
                    DWORD                           dwBytesReturned;
                    
                    // MMENDON 04-10-2001 Floppy access change STS inc.415067

                    char    szDeviceName [SYM_MAX_PATH] = "\\\\.\\";
                    char    szDosDevice [SYM_MAX_PATH];
                    TCHAR    szFloppyRootDir[4] = "?:\\";

                    szFloppyRootDir[0] = byDriveLetter;

                    // MMENDON 04-10-2001 End floppy access change 

                    szDrive[0] = byDriveLetter;
                    STRCAT(szDeviceName, szDrive);

                                                    // Get NT's device name to find
                                                    // physical drive number.
                                                    // "\\Device\\Harddisk0\\Partition1" or
                                                    // "\\Device\\Floppy0"
                                                    // Case is not consistant, thus
                                                    // STRNICMP().
                    if (0 != QueryDosDevice(szDrive, szDosDevice, sizeof(szDosDevice)))
                    {
                        
                                                        // FLOPPY DRIVE

                        if (0 == STRNICMP(szDosDevice, szFloppy, STRLEN(szFloppy)))
                        {
                            // MMENDON 04-10-2001 Floppy access change STS inc.415067

                            UINT    uErrorMode;
                            DWORD   dwCompLen = 0;
                            DWORD   dwFSFlags = 0;

                                           // prevent OS popup if no floppy in drive
                            uErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);

                            GetVolumeInformation(szFloppyRootDir, NULL, 0, NULL, &dwCompLen, 
                                                &dwFSFlags, NULL, 0);

                                            // reset the error mode: enable popups
                            uErrorMode = SetErrorMode(uErrorMode);

                            // MMENDON 04-10-2001 End floppy access change 

                            ConvertStringToLong(&dwDrive, szDosDevice + STRLEN(szFloppy));

                            stInt13Rec.generated = TRUE;
                            stInt13Rec.dwTrack   = 0;
                            stInt13Rec.dwHead    = 0;
                            stInt13Rec.dwSector  = 1;
                            stInt13Rec.dn        = (BYTE)dwDrive;
                        }

                                                    // HARD DRIVE 
                                                    // need to find offset to this partition.

                        else if ( (0 == STRNICMP(szDosDevice, szHard, STRLEN(szHard))))
                        {
                                                    // First open handle to drive.
                            hDrive = CreateFile(szDeviceName,
                                     GENERIC_READ,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL);

                            if (INVALID_HANDLE_VALUE != hDrive)
                            {
                                              
                                                    // NT supports volume sets and fault
                                                    // tolerant drives that span multiple
                                                    // physical disks. It no longer makes
                                                    // sense to ask for the dimensions of
                                                    // a logical drive. QueryDosDevice
                                                    // returns the first physical drive
                                                    // used by these volume sets. We must
                                                    // use a handle to this physical drive
                                                    // to get a drive geometry appropriate
                                                    // for finding sectors relative to the
                                                    // beginning of the drive.

                                                    // Make '\\.\PHYSICALDRIVEx' string
                                                    // using results from a previous call
                                                    // to QueryDosDevice.
                                szPhysDrive[ STRLEN(szPhysDrive) - 1 ] =
                                                            szDosDevice[ STRLEN(szHard) ];

                                                    // Get a handle to the physical drive.
                                hPhysDrive = CreateFile(szPhysDrive,
                                                        GENERIC_READ,
                                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                                        NULL,
                                                        OPEN_EXISTING,
                                                        FILE_ATTRIBUTE_NORMAL,
                                                        NULL );

                                if (INVALID_HANDLE_VALUE != hPhysDrive) 
                                {
                                    if ( (TRUE == DeviceIoControl(hDrive,
                                                  IOCTL_DISK_GET_PARTITION_INFO,
                                                  NULL, 0,
                                                  &rPartInfo, sizeof(rPartInfo),
                                                  &dwBytesReturned, NULL)) &&
                                         (TRUE == DeviceIoControl(hPhysDrive,
                                                  IOCTL_DISK_GET_DRIVE_GEOMETRY,
                                                  NULL, 0,
                                                  &rDiskGeometry, sizeof(rDiskGeometry),
                                                  &dwBytesReturned, NULL)) )
                                    {
                                 
                                                        // Get drive number.
                                        ConvertStringToLong(&dwDrive, szDosDevice + STRLEN(szHard));

                                        dwSectorOffset = rPartInfo.pi.StartingOffset.LowPart
                                                         / rDiskGeometry.BytesPerSector;

                                        dwTrackOffset = dwSectorOffset
                                                        / rDiskGeometry.SectorsPerTrack;

                                                        // Compute location of sector 0.
                                        stInt13Rec.generated = TRUE;
  
                                        stInt13Rec.dwTrack =
                                            (WORD)(dwTrackOffset / rDiskGeometry.TracksPerCylinder);

                                        stInt13Rec.dwHead =
                                            (BYTE)(dwTrackOffset % rDiskGeometry.TracksPerCylinder);
 
                                        stInt13Rec.dwSector =
                                            1 + (BYTE)(dwSectorOffset % rDiskGeometry.SectorsPerTrack);

                                        stInt13Rec.dn = (BYTE)(dwDrive + FIRST_HD);

                                    }   // end if (IOCTL_GET_PARTITION_INFO and
                                        //         IOCTL_GET_DRIVE_GEOMETRY)

                                    CloseHandle ( hPhysDrive );

                                }  //  if (INVALID_HANDLE != hPhysDrive)

                                CloseHandle ( hDrive );

                            }  //  if (INVALID_HANDLE_VALUE != hDrive)
                            
                            else  // CreateFile(hFile) failed
                            {
                                return (ERR);
                            }

                        }   // else if(hard disk)

                        else         // Not floppy are hard disk
                        {
                            return (ERR);
                        }

                    }//if (0 != QueryDosDevice())

                    else            //  QueryDosDevice() failed
                    {
                        return (ERR);
                    }

                }//end else NT 4.0 Version



            }//end if (VER_PLATFORM_WIN32_NT == OSVersionInfo.dwPlatformId)
        }//end if (TRUE == bOsVersionInfoValid)
    }//end if(SYM_SYSTEM_WIN_NT == SystemGetWindowsType())
    else
#endif  // SYM_WIN32

    DiskMapLogToPhyParams(0,
                          byDriveLetter,
                          &stInt13Rec);

    if (!stInt13Rec.generated)          // do we have physical info?
    {
        return (ERR);
    }

    BOOL bPartitioned;

    if (NAVLinearIsPartitionedMedia((LPVOID)stInt13Rec.dn, &bPartitioned) != CBSTATUS_OK)
    {
        return (ERR);
    }

    if (!bPartitioned)
    {
        *lpbyDriveNumber = stInt13Rec.dn;
        *lpbyPartEntry   = 0;
        return (NOERR);
    }

    MEMSET(&stAbsDisk, 0, sizeof(stAbsDisk));

    stAbsDisk.dn = stInt13Rec.dn;

    DiskGetPhysicalInfo(&stAbsDisk);

    stAbsDisk.dwHead     = 0;
    stAbsDisk.dwTrack    = 0;
    stAbsDisk.dwSector   = 1;
    stAbsDisk.numSectors = 1;
    stAbsDisk.buffer     = (HPBYTE)MemAllocPtr(GHND, 8 * PHYSICAL_SECTOR_SIZE);

    if (!stAbsDisk.buffer)
    {
        return (ERR);
    }
                                        // read the MBR

   // 6/16/1999: Mike Keating: This was crashing on Magento-Optical drives.
   // Possibly because the buffer is on aligned on a 4k boundry. Calling
   // NAVEXSDiskReadPhysical which does to 4k alignment.
   // if (DiskAbsOperation(READ_COMMAND, &stAbsDisk))
   // {
   //     MemFreePtr(stAbsDisk.buffer);
   //     return (ERR);
   // }

    if (NAVEXSDiskReadPhysical(stAbsDisk.buffer, 1, 0, 1, 0, stInt13Rec.dn))
    {
        MemFreePtr(stAbsDisk.buffer);
        return (ERR);
    }
                                        // Get the linear offset of our boot record

    dwLinearBootSector = ConvertPhySectorToLong(&stAbsDisk,
                                                stInt13Rec.dwHead,
                                                stInt13Rec.dwTrack,
                                                stInt13Rec.dwSector);

                                        // find the partition entry

    for (byPartEntry = 0; byPartEntry < 4; byPartEntry++)
    {
        LPPARTENTRYREC lpPartEntryRec =
            (LPPARTENTRYREC)(stAbsDisk.buffer + PART_TABLE_OFFSET) +
            byPartEntry;

                                        // Is this an unused partition entry?

        if (lpPartEntryRec->system == SYSTEM_NOT_USED)
            continue;

                                        // Is our boot sector within this
                                        // partition?

        if (dwLinearBootSector >= lpPartEntryRec->relativeSectors &&
            dwLinearBootSector < (lpPartEntryRec->relativeSectors +
                                  lpPartEntryRec->numSectors))
        {
            *lpbyDriveNumber = stInt13Rec.dn;
            *lpbyPartEntry   = byPartEntry;    // found it!

            MemFreePtr(stAbsDisk.buffer);
            return (NOERR);
        }
    }

    MemFreePtr(stAbsDisk.buffer);
    return (ERR);
}

#endif



