#pragma once

#include "CmPrerequisites.h"
#include "CmPixelData.h"

namespace CamelotEngine
{
	class CM_EXPORT DepthStencilBuffer
	{
	public:
		DepthStencilBuffer(DepthStencilFormat format, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint);
		virtual ~DepthStencilBuffer();

		DepthStencilFormat getFormat() const { return mFormat; }
		UINT32 getWidth() const { return mWidth; }
		UINT32 getHeight() const { return mHeight; }
		UINT32 getFsaa() const { return mFsaa; }
		const String& getFsaaHint() const { return mFsaaHint; }

		/** Returns whether the specified RenderTarget is compatible with this DepthBuffer
			That is, this DepthBuffer can be attached to that RenderTarget
            @note
                Most APIs impose the following restrictions:
				Width & height must be equal or higher than the render target's
				They must be of the same bit depth.
				They need to have the same FSAA setting
        */
		virtual bool isCompatible(RenderTarget *renderTarget) const;

	protected:
		DepthStencilFormat			mFormat;
		UINT32						mWidth;
		UINT32						mHeight;
		UINT32						mFsaa;
		String						mFsaaHint;
	};
}