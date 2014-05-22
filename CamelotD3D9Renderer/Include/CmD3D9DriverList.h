#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmD3D9Driver.h"

namespace BansheeEngine 
{
	class CM_D3D9_EXPORT D3D9DriverList
	{
	public:
		D3D9DriverList();
		~D3D9DriverList();

		UINT32 count() const;

		D3D9Driver* item(UINT32 index);
		D3D9Driver* item(const String &name);

		VideoModeInfoPtr getVideoModeInfo() const { return mVideoModeInfo; }

	private:
		Vector<D3D9Driver> mDriverList;
		VideoModeInfoPtr mVideoModeInfo;
	};
}