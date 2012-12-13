#include "CmD3D11VideoMode.h"

namespace CamelotEngine
{
	D3D11VideoMode::D3D11VideoMode(DXGI_OUTPUT_DESC d3ddm, DXGI_MODE_DESC modeDesc)
		:mDisplayMode(d3ddm), mModeDesc(modeDesc)
	{ }

	D3D11VideoMode::D3D11VideoMode(const D3D11VideoMode &ob)
		:mDisplayMode(ob.mDisplayMode), mModeDesc(ob.mModeDesc)
	{ }

	D3D11VideoMode::D3D11VideoMode()
	{
		ZeroMemory(&mDisplayMode, sizeof(DXGI_OUTPUT_DESC));
		ZeroMemory(&mModeDesc, sizeof(DXGI_MODE_DESC));
	}

	D3D11VideoMode::~D3D11VideoMode()
	{ }

	String D3D11VideoMode::getDescription() const
	{
		return toString(mModeDesc.Width) + " x " + toString(mModeDesc.Height) + " @ " + toString(getColorDepth()) + "-bit color";
	}

	UINT32 D3D11VideoMode::getColorDepth() const
	{
		UINT32 colourDepth = 16;
		if( mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_TYPELESS ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_UINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32A32_SINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_TYPELESS ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_FLOAT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_UINT ||
			mModeDesc.Format == DXGI_FORMAT_R32G32B32_SINT 
			)
			colourDepth = 32;

		return colourDepth;
	}
}