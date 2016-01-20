//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPrerequisitesUtil.h"
#include <windows.h>
#include <iphlpapi.h>

namespace BansheeEngine
{
	void PlatformUtility::terminate(bool force)
	{
		if (!force)
			PostQuitMessage(0);
		else
			TerminateProcess(GetCurrentProcess(), 0);
	}

	double PlatformUtility::queryPerformanceTimerMs()
	{
		LARGE_INTEGER counterValue;
		QueryPerformanceCounter(&counterValue);

		LARGE_INTEGER counterFreq;
		QueryPerformanceFrequency(&counterFreq);

		return (double)counterValue.QuadPart / (counterFreq.QuadPart * 0.001);
	}


	void PlatformUtility::copyToClipboard(const WString& string)
	{
		HANDLE hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (string.size() + 1) * sizeof(WString::value_type));
		WString::value_type* buffer = (WString::value_type*)GlobalLock(hData);

		string.copy(buffer, string.size());
		buffer[string.size()] = '\0';

		GlobalUnlock(hData);

		if (OpenClipboard(NULL))
		{
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hData);
			CloseClipboard();
		}
		else
		{
			GlobalFree(hData);
		}
	}

	WString PlatformUtility::copyFromClipboard()
	{
		if (OpenClipboard(NULL))
		{
			HANDLE hData = GetClipboardData(CF_UNICODETEXT);

			if (hData != NULL)
			{
				WString::value_type* buffer = (WString::value_type*)GlobalLock(hData);
				WString string(buffer);
				GlobalUnlock(hData);

				CloseClipboard();
				return string;
			}
			else
			{
				CloseClipboard();
				return L"";
			}
		}

		return L"";
	}

	WString PlatformUtility::keyCodeToUnicode(UINT32 keyCode)
	{
		static HKL keyboardLayout = GetKeyboardLayout(0);
		static UINT8 keyboarState[256];

		if (GetKeyboardState(keyboarState) == FALSE)
			return 0;

		UINT virtualKey = MapVirtualKeyExW(keyCode, 1, keyboardLayout);

		wchar_t output[2];
		int count = ToUnicodeEx(virtualKey, keyCode, keyboarState, output, 2, 0, keyboardLayout);
		if (count > 0)
			return WString(output, count);

		return StringUtil::WBLANK;
	}

	bool PlatformUtility::getMACAddress(MACAddress& address)
	{
		std::memset(&address, 0, sizeof(address));

		PIP_ADAPTER_INFO adapterInfo = bs_alloc<IP_ADAPTER_INFO>();

		ULONG len = sizeof(IP_ADAPTER_INFO);
		DWORD rc = GetAdaptersInfo(adapterInfo, &len);

		if (rc == ERROR_BUFFER_OVERFLOW)
		{
			bs_free(adapterInfo);
			adapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(bs_alloc(len));
		}
		else if (rc != ERROR_SUCCESS)
		{
			bs_free(adapterInfo);
			return false;
		}

		if (GetAdaptersInfo(adapterInfo, &len) == NO_ERROR)
		{
			PIP_ADAPTER_INFO curAdapter = nullptr;
			curAdapter = adapterInfo;

			while (curAdapter)
			{
				if (curAdapter->Type == MIB_IF_TYPE_ETHERNET && curAdapter->AddressLength == sizeof(address))
				{
					std::memcpy(&address, curAdapter->Address, curAdapter->AddressLength);
					return true;
				}

				curAdapter = curAdapter->Next;
			}
		}

		bs_free(adapterInfo);
		return false;
	}

	String PlatformUtility::generateUUID()
	{
		UUID uuid;
		UuidCreate(&uuid);

		UINT8* uuidStr;
		UuidToStringA(&uuid, &uuidStr);

		String output((char*)uuidStr);
		RpcStringFreeA(&uuidStr);

		return output;
	}

	void PlatformUtility::open(const Path& path)
	{
		ShellExecute(nullptr, "open", path.toString().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	}
}