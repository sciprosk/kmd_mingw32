#include <ntddk.h>

#define TEST_DRIVER_TRACE 0x8000
#define IOCTL_TEST_DRIVER_TRACE CTL_CODE(TEST_DRIVER_TRACE, 0x800,\
		METHOD_NEITHER, FILE_ANY_ACCESS)

static NTSTATUS test_driver_create_close(PDEVICE_OBJECT devobj, PIRP ipr);
static NTSTATUS test_driver_ioctl(PDEVICE_OBJECT devobj, PIRP irp);
static void test_driver_unload(PDRIVER_OBJECT driverObject);

NTSTATUS 
DriverEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath)
{
	UNREFERENCED_PARAMETER(registryPath);
	DbgPrint("Sample driver initialized successfully\n");

	driverObject->DriverUnload = test_driver_unload;
	driverObject->MajorFunction[IRP_MJ_CREATE] = test_driver_create_close;
	driverObject->MajorFunction[IRP_MJ_CLOSE] = test_driver_create_close;
	driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = test_driver_ioctl;

	UNICODE_STRING devName = RTL_CONSTANT_STRING(L"\\Device\\test_driver");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\test_driver");
	PDEVICE_OBJECT deviceObject = NULL;
	NTSTATUS status = STATUS_SUCCESS;

	do {
		status = IoCreateDevice(driverObject, 0, &devName,
				FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
		if (!NT_SUCCESS(status)) {
			DbgPrint("Failed to create device (0x%08X)\n", status);
			break;
		}
		if (!deviceObject) {
			DbgPrint("Unexpected nullptr for the device object\n");
			break;
		}
		
		status = IoCreateSymbolicLink(&symLink, &devName);
		if (!NT_SUCCESS(status))
		{
			DbgPrint("Failed to create symbolic link (0x%08X)\n", status);
			break;
		}
	} while (0);


	if (!NT_SUCCESS(status))  {
		if (deviceObject)
			IoDeleteDevice(deviceObject);
	}
	return status;
}

NTSTATUS
test_driver_create_close(PDEVICE_OBJECT devobj, PIRP irp)
{
	UNREFERENCED_PARAMETER(devobj);
	DbgPrint("Driver CreateClose called\n");
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS
test_driver_ioctl(PDEVICE_OBJECT devobj, PIRP irp)
{
	UNREFERENCED_PARAMETER(devobj);
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
	case IOCTL_TEST_DRIVER_TRACE:
		DbgPrint("Received ioctl %08X\n", IOCTL_TEST_DRIVER_TRACE);
		break;
	default:
		DbgPrint("Invalid ioctl code received\n");
		status = STATUS_INVALID_DEVICE_REQUEST;
		break;
	}
	irp->IoStatus.Status = status;
	irp->IoStatus.Information = 0;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return status;
}

void
test_driver_unload(PDRIVER_OBJECT driverObject)
{
	UNREFERENCED_PARAMETER(driverObject);
	DbgPrint("Driver unload called\n");
	UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\??\\test_driver");
	IoDeleteSymbolicLink(&symLink);
	if (driverObject->DeviceObject)
		IoDeleteDevice(driverObject->DeviceObject);
}

