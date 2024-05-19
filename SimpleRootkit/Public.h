/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_SimpleRootkit,
    0x3cdbe11a,0x1bef,0x4fa0,0xaf,0x42,0x65,0x0c,0xbe,0x1a,0x54,0x59);
// {3cdbe11a-1bef-4fa0-af42-650cbe1a5459}
