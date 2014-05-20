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
		struct RefreshRate
		{
			UINT32 numerator;
			UINT32 denominator;
		};

	public:
		D3D11VideoMode(UINT32 width, UINT32 height, VideoOutputInfo* outputInfo, DXGI_MODE_DESC dxgiMode);

		/**
		 * @brief	Returns an internal DXGI representation of this video mode.
		 */
		const DXGI_MODE_DESC& getDXGIModeDesc() const { return mDXGIMode; }

	private:
		friend class D3D11VideoOutputInfo;

		Vector<RefreshRate> mD3D11RefreshRates; /**< DXGI stores refresh rates as rational numbers so we need a special storage for them. */
		DXGI_MODE_DESC mDXGIMode;
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