#include <windows.h>

#include <iostream>

#define TEST_DRIVER_TRACE 0x8000
#define IOCTL_TEST_DRIVER_TRACE CTL_CODE(TEST_DRIVER_TRACE, 0x800,\
		METHOD_NEITHER, FILE_ANY_ACCESS)

class ScopedOpenDevice
{
public:
	explicit ScopedOpenDevice(const char* deviceName)
	{
		device_ = CreateFile(deviceName, GENERIC_WRITE,
				FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
		err_ = GetLastError();
	}
	~ScopedOpenDevice()
	{
		if (INVALID_HANDLE_VALUE != device_)
			CloseHandle(device_);
	}

	explicit operator bool() const noexcept
	{
		return INVALID_HANDLE_VALUE != device_;
	}

	HANDLE operator()() const noexcept
	{
		return device_;
	}

	DWORD err() const noexcept
	{
		return err_;
	}

private:
	HANDLE device_;
	DWORD err_;
};

int main()
{
	ScopedOpenDevice device("\\\\.\\test_driver");
	if (!device) {
		std::cout << "Failed to open device, err = "
			<< device.err() << std::endl;
		return 0;
	}

	auto success = DeviceIoControl(device(), IOCTL_TEST_DRIVER_TRACE,
			nullptr, 0, nullptr, 0, nullptr, nullptr);
	if (!success) {
		std::cout << "Failed to send ioctl, err = "
			<< GetLastError() << std::endl;
	}
	
	return 0;
}
