#pragma once

#include "CmD3D11Prerequisites.h"

namespace BansheeEngine
{
	class CM_D3D11_EXPORT D3D11VideoMode
	{
	public:
		D3D11VideoMode();
		D3D11VideoMode(const D3D11VideoMode& ob);
		D3D11VideoMode(DXGI_OUTPUT_DESC d3ddm, DXGI_MODE_DESC modeDesc);
		~D3D11VideoMode();

		UINT32 getWidth() const { return mModeDesc.Width; }
		UINT32 getHeight() const { return mModeDesc.Height; }
		DXGI_FORMAT getFormat() const { return mModeDesc.Format; }
		DXGI_RATIONAL getRefreshRate() const { return mModeDesc.RefreshRate; }
		UINT32 getColorDepth() const;
		DXGI_OUTPUT_DESC getDisplayMode() const { return mDisplayMode; }
		DXGI_MODE_DESC getModeDesc() const { return mModeDesc; }
		String getDescription() const;

	private:
		DXGI_OUTPUT_DESC	mDisplayMode;
		DXGI_MODE_DESC		mModeDesc;
		UINT32				modeNumber;
	};
}