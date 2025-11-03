#include <ntddk.h>

static void test_driver_unload(PDRIVER_OBJECT driverObject);

NTSTATUS 
DriverEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath)
{
	UNREFERENCED_PARAMETER(registryPath);
	DbgPrint("Sample driver initialized successfully\n");

	driverObject->DriverUnload = test_driver_unload;
	
	return STATUS_SUCCESS;
}

void
test_driver_unload(PDRIVER_OBJECT driverObject)
{
	UNREFERENCED_PARAMETER(driverObject);
	DbgPrint("Driver unload called\n");
}

