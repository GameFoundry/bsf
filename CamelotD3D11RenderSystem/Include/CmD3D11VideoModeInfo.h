#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmVideoModeInfo.h"

namespace BansheeEngine
{
	/**
	 * @copydoc	VideoMode
	 */
	class CM_D3D11_EXPORT D3D11VideoMode : public VideoMode
	{
		struct DX11Data
		{
			UINT32 refreshRateNumerator;
			UINT32 refreshRateDenominator;
			DXGI_MODE_DESC mode;
		};

	public:
		D3D11VideoMode(UINT32 width, UINT32 height, VideoOutputInfo* outputInfo);

		/**
		 * @brief	Returns an internal DXGI representation of this video mode.
		 *
		 * @param	idx		Index referencing which refresh rate to use.
		 */
		const DXGI_MODE_DESC& getDXGIModeDesc(UINT32 idx) const { return mD3D11Modes[idx].mode; }

	private:
		friend class D3D11VideoOutputInfo;

		Vector<DX11Data> mD3D11Modes; 
	};

	/**
	* @copydoc	VideoOutputInfo
	*/
	class CM_D3D11_EXPORT D3D11VideoOutputInfo : public VideoOutputInfo
	{
	public:
		D3D11VideoOutputInfo(IDXGIOutput* output);
		~D3D11VideoOutputInfo();

		/**
		 * @brief	Returns the internal DXGI object representing an output device.
		 */
		IDXGIOutput* getDXGIOutput() const { return mDXGIOutput;  }

	private:
		IDXGIOutput* mDXGIOutput;
	};

	/**
	* @copydoc	VideoModeInfo
	*/
	class CM_D3D11_EXPORT D3D11VideoModeInfo : public VideoModeInfo
	{
	public:
		D3D11VideoModeInfo(IDXGIAdapter* dxgiAdapter);
	};
}