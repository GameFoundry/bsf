#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class CM_EXPORT DepthStencilBuffer
	{
	public:
		DepthStencilBuffer(UINT32 bitDepth, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint);
		virtual ~DepthStencilBuffer();

		UINT32 getBitDepth() const { return mBitDepth; }
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
		UINT32						mBitDepth;
		UINT32						mWidth;
		UINT32						mHeight;
		UINT32						mFsaa;
		String						mFsaaHint;

		void detachFromAllRenderTargets();
	};
}