// Copyright 1996 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVEX/VCS/NAVEXNTL.CPv   1.3   26 Dec 1996 15:22:16   AOONWAL  $
//
// Description:
//      This file contains functions for initializing/deinitializing NAVEX
//      for SYM_NTK platform.  These functions are statically linked to
//      AVAPI, and are AVAPI's interface to NAVEX.
//
// Contains:
//      EXTInitNTK()
//      EXTCloseNTK()
//
// Note:
//      Supports SYM_NTK only!
//
//***************************************************************************
// $Log:   S:/NAVEX/VCS/NAVEXNTL.CPv  $
// 
//    Rev 1.3   26 Dec 1996 15:22:16   AOONWAL
// No change.
// 
//    Rev 1.2   02 Dec 1996 14:01:28   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:58:28   AOONWAL
// No change.
// 
//    Rev 1.0   19 Apr 1996 14:34:04   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "drvcomm.h"
#include "avapi.h"
#include "navex.h"
#include "navexnt.h"

#if !defined(SYM_NTK)
#error This file supports only SYM_NTK platform!
#endif

//---------------------------------------------------------------------------
//
// FileObject is used to lock the NAVEX device object in memory.
//
//---------------------------------------------------------------------------
static PFILE_OBJECT FileObject = NULL;

//---------------------------------------------------------------------------
//
// EXTSTATUS WINAPI EXTInitNTK (
//      HVCONTEXT hContext
//      );
//
// This function initializes the NAVEX engine, and makes it available to
// hContext.  No other hContext can be using NAVEX when EXTInitNTK() is
// called.  This means, that once EXTInitNTK() was successfully called,
// you can only call EXTCloseNTK().
//
// Entry:
//      hContext - AVAPI context handle.
//
// Exit:
//      EXTSTATUS_OK if successful, otherwise error.
//
// Note:
//      Currently this function returns only EXTSTATUS_OK.
//
//---------------------------------------------------------------------------
EXTSTATUS WINAPI EXTInitNTK (
    HVCONTEXT hContext
    )
    {
    auto PIRP               Irp;
    auto UNICODE_STRING     ObjectName;
    auto PDEVICE_OBJECT     DeviceObject;
    auto KEVENT             event;
    auto IO_STATUS_BLOCK    IoStatusBlock;
    auto PEXPORT_TABLE_TYPE ExportTable;

                                        // Validate input parameters.

    SYM_ASSERT ( hContext );

                                        // Assume NAVEX not found.

    hContext->prExportTable = NULL;

                                        // Get the NAVEX device object.

    RtlInitUnicodeString ( &ObjectName, szNAVEXDeviceName );

    if ( NT_SUCCESS ( IoGetDeviceObjectPointer ( &ObjectName,
                                                 FILE_ALL_ACCESS,
                                                 &FileObject,
                                                 &DeviceObject ) ) )
        {
        if ( DeviceObject )
            {
                                        // Allocate Irp for DIOCTL

            Irp = IoBuildDeviceIoControlRequest ( DIOCTL_SYM_USER01,
                                                  DeviceObject,
                                                  NULL,
                                                  0,
                                                  &ExportTable,
                                                  sizeof(ExportTable),
                                                  FALSE,
                                                  &event,
                                                  &IoStatusBlock );

            if ( Irp )
                {
                                        // Init an event object to wait on.

                KeInitializeEvent ( &event, NotificationEvent, FALSE );

                switch ( IoCallDriver ( DeviceObject, Irp ) )
                    {
                    case STATUS_PENDING:

                        KeWaitForSingleObject ( &event,
                                                Suspended,
                                                KernelMode,
                                                FALSE,
                                                NULL );

                        if ( !NT_SUCCESS ( IoStatusBlock.Status ) )
                            {
                            SYM_ASSERT ( FALSE );
                            break;
                            }

                    case STATUS_SUCCESS:

                        if ( NT_SUCCESS ( IoStatusBlock.Status ) &&
                             IoStatusBlock.Information == sizeof(ExportTable) )
                            {
                            SYM_ASSERT ( ExportTable );
                            hContext->prExportTable = ExportTable;
                            }

                        break;

                    default:
                        SYM_ASSERT ( FALSE );
                    }
                }
            else
                {
                SYM_ASSERT ( FALSE );
                }
            }
        else
            {
            SYM_ASSERT ( FALSE );
            }
        }
    else
        {
        SYM_ASSERT ( FALSE );
        }

    return ( EXTSTATUS_OK );
    }

//---------------------------------------------------------------------------
//
// EXTSTATUS WINAPI EXTCloseNTK (
//      HVCONTEXT hContext
//      );
//
// This function deinitializes the NAVEX engine for hContext.
//
// Entry:
//      hContext - AVAPI context handle, to which EXTInitNTK() has
//                 successfully added NAVEX support.
//
// Exit:
//      EXTSTATUS_OK
//
// Synchronization:
//      This function is non-reentrant.
//
//---------------------------------------------------------------------------
EXTSTATUS WINAPI EXTCloseNTK (
    HVCONTEXT hContext
    )
    {
                                        // Dereference the handle to the file
                                        // object.

    if ( FileObject )
        {
        ObDereferenceObject ( FileObject );

        FileObject = NULL;
        }

                                        // Invalidate the NAVEX exports in
                                        // hContext.

    hContext->prExportTable = NULL;

    return ( EXTSTATUS_OK );
    }
