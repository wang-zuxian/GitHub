#pragma once
class CDeviceManager
{
public:
	CDeviceManager();
	~CDeviceManager();
	bool EjectDeviceByPath(const WCHAR * _pwcsDevPath);
	bool EjectUsbStorge();
};

