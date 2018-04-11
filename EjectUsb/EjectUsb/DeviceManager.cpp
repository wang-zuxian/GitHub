#include "stdafx.h"
#include "DeviceManager.h"
#include <Windows.h>
#include <WinIoCtl.h>

CDeviceManager::CDeviceManager()
{
}


CDeviceManager::~CDeviceManager()
{
}


bool CDeviceManager::EjectDeviceByPath(const WCHAR * _pwcsDevPath)
{
	WCHAR wcsPath[MAX_PATH] = { 0 };
	HANDLE hDevice = INVALID_HANDLE_VALUE;
	DWORD dwBytesReturned = 0;
	// _pszDevPath有种格式如:\\?\usbstor#disk&ven_sandisk...和G:
	if ( _pwcsDevPath == NULL || wcsncmp(_pwcsDevPath + 4, L"usbstor", 7) != 0 )
	{
		return false;
	}
	if (_pwcsDevPath[0] > 'A' && _pwcsDevPath[0]<'Z' || _pwcsDevPath[0]>'a' && _pwcsDevPath[0] < 'z')
	{
		wprintf_s(wcsPath, sizeof(wcsPath) - 1, L"\\\\.\\%c:", _pwcsDevPath[0]);
	}
	else
	{
		wcsncpy_s(wcsPath, _pwcsDevPath, sizeof(wcsPath) - 1);
	}

	hDevice = CreateFile(wcsPath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	dwBytesReturned = 0;
	PREVENT_MEDIA_REMOVAL PMRBuffer;
	PMRBuffer.PreventMediaRemoval = FALSE;
	if ( !DeviceIoControl(hDevice, IOCTL_STORAGE_MEDIA_REMOVAL, &PMRBuffer, sizeof(PREVENT_MEDIA_REMOVAL), NULL, 0, &dwBytesReturned, NULL) )
	{
		// warm
	}
	dwBytesReturned = 0;
	if ( !DeviceIoControl(hDevice, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &dwBytesReturned, NULL) )
	{
		CloseHandle(hDevice);
		return false;
	}

	CloseHandle(hDevice);
	return true;
}

bool CDeviceManager::EjectUsbStorge()
{
	WCHAR wcsDevice[256] = { 0 };
	WCHAR wcsDevPath[10] = { 0 };
	DWORD dwRet = 0;
	PWCHAR pwcsDevice = NULL;
	dwRet = GetLogicalDriveStrings(sizeof(wcsDevice) - 1, wcsDevice); // 返回格式如C:\<null>D:\<null><null>
	if (0 == dwRet)
	{
		return false;
	}
	else if (dwRet > sizeof(wcsDevice) - 1)
	{
		return false;
	}
	WORD count = 0;
	pwcsDevice = wcsDevice;
	while (1)
	{
		switch (GetDriveType(pwcsDevice))
		{
		case DRIVE_REMOVABLE:
			EjectDeviceByPath(pwcsDevice);
		default:
			break;
		}
		if (&pwcsDevice == '\0' || count >= 26)
		{
			break;
		}
		pwcsDevice = pwcsDevice + 4;
		count++;
	}
	return true;
}
