#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptRenderTarget.h"
#include "BsPixelUtil.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptRenderTexture2D : public ScriptObject < ScriptRenderTexture2D, ScriptRenderTargetBase >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "RenderTexture2D")

		RenderTargetPtr getNativeValue() const { return mRenderTarget; }
		RenderTexturePtr getRenderTexture() const;
		MultiRenderTexturePtr getMultiRenderTexture() const;

	private:
		ScriptRenderTexture2D(const RenderTargetPtr& target, bool isMulti, MonoObject* instance);

		static void internal_createDetailed(MonoObject* instance, PixelFormat format, UINT32 width, UINT32 height, 
			UINT32 numSamples, bool gammaCorrection, bool createDepth, PixelFormat depthStencilFormat);
		static void internal_create(MonoObject* instance, MonoArray* colorSurfaces, MonoObject* depthStencilSurface);

		static void internal_getColorSurfaces(ScriptRenderTexture2D* thisPtr, MonoArray** value);
		static void internal_getDepthStencilSurface(ScriptRenderTexture2D* thisPtr, MonoObject** value);

		RenderTargetPtr mRenderTarget;
		bool mIsMulti;
	};
}