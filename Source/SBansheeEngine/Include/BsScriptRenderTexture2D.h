//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptRenderTarget.h"
#include "BsPixelUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for a single or multi RenderTexture using a 2D texture.
	 */
	class BS_SCR_BE_EXPORT ScriptRenderTexture2D : public ScriptObject < ScriptRenderTexture2D, ScriptRenderTargetBase >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "RenderTexture2D")

		/**
		 * @copydoc	ScriptRenderTargetBase::getNativeValue
		 */
		RenderTargetPtr getNativeValue() const override { return mRenderTarget; }

		/**
		 * @brief	Returns the internal wrapped render texture. Returns null if this object
		 *			instead wraps a multi render target.
		 */
		RenderTexturePtr getRenderTexture() const;

		/**
		 * @brief	Returns the internal wrapped multi render texture. Returns null if this object
		 *			instead wraps a single render target.
		 */
		MultiRenderTexturePtr getMultiRenderTexture() const;

	private:
		ScriptRenderTexture2D(const RenderTargetPtr& target, bool isMulti, MonoObject* instance);

		RenderTargetPtr mRenderTarget;
		bool mIsMulti;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createDetailed(MonoObject* instance, PixelFormat format, UINT32 width, UINT32 height,
			UINT32 numSamples, bool gammaCorrection, bool createDepth, PixelFormat depthStencilFormat);
		static void internal_create(MonoObject* instance, MonoArray* colorSurfaces, MonoObject* depthStencilSurface);

		static void internal_getColorSurfaces(ScriptRenderTexture2D* thisPtr, MonoArray** value);
		static void internal_getDepthStencilSurface(ScriptRenderTexture2D* thisPtr, MonoObject** value);
	};
}