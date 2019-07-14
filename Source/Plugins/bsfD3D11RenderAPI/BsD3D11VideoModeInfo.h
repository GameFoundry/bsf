//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "RenderAPI/BsVideoModeInfo.h"

namespace bs { namespace ct
{
	/** @addtogroup D3D11
	 *  @{
	 */

	/** @copydoc VideoMode */
	class D3D11VideoMode : public VideoMode
	{
	public:
		D3D11VideoMode(UINT32 width, UINT32 height, float refreshRate, UINT32 outputIdx, UINT32 refreshRateNumerator,
			UINT32 refreshRateDenominator, DXGI_MODE_DESC mode);

		/**	Returns an internal DXGI representation of this video mode. */
		const DXGI_MODE_DESC& getDXGIModeDesc() const { return mD3D11Mode; }

		/**	Gets internal DX11 refresh rate numerator. */
		UINT32 getRefreshRateNumerator() const { return mRefreshRateNumerator; }

		/**	Gets internal DX11 refresh rate denominator. */
		UINT32 getRefreshRateDenominator() const { return mRefreshRateDenominator; }

	private:
		friend class D3D11VideoOutputInfo;

		UINT32 mRefreshRateNumerator;
		UINT32 mRefreshRateDenominator;
		DXGI_MODE_DESC mD3D11Mode;
	};

	/** @copydoc VideoOutputInfo */
	class D3D11VideoOutputInfo : public VideoOutputInfo
	{
	public:
		D3D11VideoOutputInfo(IDXGIOutput* output, UINT32 outputIdx);
		~D3D11VideoOutputInfo();

		/**	Returns the internal DXGI object representing an output device. */
		IDXGIOutput* getDXGIOutput() const { return mDXGIOutput;  }

	private:
		IDXGIOutput* mDXGIOutput;
	};

	/** @copydoc VideoModeInfo */
	class D3D11VideoModeInfo : public VideoModeInfo
	{
	public:
		D3D11VideoModeInfo(IDXGIAdapter* dxgiAdapter);
	};

	/** @} */
}}
