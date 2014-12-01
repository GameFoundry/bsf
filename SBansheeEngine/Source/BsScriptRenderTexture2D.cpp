#include "BsScriptRenderTexture2D.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsRenderTexture.h"
#include "BsScriptTexture2D.h"
#include "BsMultiRenderTexture.h"
#include "BsMonoUtil.h"
#include "BsScriptResourceManager.h"

namespace BansheeEngine
{
	ScriptRenderTexture2D::ScriptRenderTexture2D(const RenderTargetPtr& target, bool isMulti, MonoObject* instance)
		:ScriptObject(instance), mRenderTarget(target), mIsMulti(isMulti)
	{

	}

	void ScriptRenderTexture2D::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateDetailed", &ScriptRenderTexture2D::internal_createDetailed);
		metaData.scriptClass->addInternalCall("Internal_Create", &ScriptRenderTexture2D::internal_create);
		metaData.scriptClass->addInternalCall("Internal_GetColorSurfaces", &ScriptRenderTexture2D::internal_getColorSurfaces);
		metaData.scriptClass->addInternalCall("Internal_GetDepthStencilSurface", &ScriptRenderTexture2D::internal_getDepthStencilSurface);
	}

	RenderTexturePtr ScriptRenderTexture2D::getRenderTexture() const
	{
		if (!mIsMulti)
			return std::static_pointer_cast<RenderTexture>(mRenderTarget);

		return nullptr;
	}

	MultiRenderTexturePtr ScriptRenderTexture2D::getMultiRenderTexture() const
	{
		if (mIsMulti)
			return std::static_pointer_cast<MultiRenderTexture>(mRenderTarget);

		return nullptr;
	}

	void ScriptRenderTexture2D::internal_createDetailed(MonoObject* instance, PixelFormat format, UINT32 width, UINT32 height,
		UINT32 numSamples, bool gammaCorrection, bool createDepth, PixelFormat depthStencilFormat)
	{
		RenderTexturePtr tex = RenderTexture::create(TEX_TYPE_2D, width, height, format, gammaCorrection, numSamples, createDepth, depthStencilFormat);

		ScriptRenderTexture2D* scriptResource = new (bs_alloc<ScriptRenderTexture2D>()) ScriptRenderTexture2D(tex, false, instance);
	}

	void ScriptRenderTexture2D::internal_create(MonoObject* instance, MonoArray* colorSurfaces, MonoObject* depthStencilSurface)
	{
		ScriptArray colorSurfacesList(colorSurfaces);

		RENDER_SURFACE_DESC depthStencilSurfaceDesc;
		ScriptTexture2D* scriptDepthStencilSurface = ScriptTexture2D::toNative(depthStencilSurface);
		if (scriptDepthStencilSurface != nullptr)
		{
			depthStencilSurfaceDesc.face = 0;
			depthStencilSurfaceDesc.mipLevel = 0;

			HTexture textureHandle = scriptDepthStencilSurface->getTextureHandle();
			if (!textureHandle.isLoaded())
			{
				LOGERR("Render texture must be created using a fully loaded texture.");
			}
			else
				depthStencilSurfaceDesc.texture = textureHandle.getInternalPtr();
		}

		RenderTargetPtr tex;

		UINT32 numSurfaces = colorSurfacesList.size();
		bool isMulti = numSurfaces > 1;
		if (isMulti)
		{
			MULTI_RENDER_TEXTURE_DESC desc;

			for (UINT32 i = 0; i < numSurfaces; i++)
			{
				RENDER_SURFACE_DESC surfaceDesc;
				surfaceDesc.face = 0;
				surfaceDesc.mipLevel = 0;

				MonoObject* surfaceObj = colorSurfacesList.get<MonoObject*>(i);
				ScriptTexture2D* scriptSurface = ScriptTexture2D::toNative(surfaceObj);

				if (scriptSurface != nullptr)
				{
					HTexture textureHandle = scriptSurface->getTextureHandle();
					if (!textureHandle.isLoaded())
					{
						LOGERR("Render texture must be created using a fully loaded texture.");
					}
					else
						surfaceDesc.texture = textureHandle.getInternalPtr();
				}

				desc.colorSurfaces.push_back(surfaceDesc);
			}

			desc.depthStencilSurface = depthStencilSurfaceDesc;

			tex = MultiRenderTexture::create(desc);
		}
		else
		{
			RENDER_SURFACE_DESC surfaceDesc;
			surfaceDesc.face = 0;
			surfaceDesc.mipLevel = 0;

			if (colorSurfacesList.size() > 0)
			{
				MonoObject* surfaceObj = colorSurfacesList.get<MonoObject*>(0);
				ScriptTexture2D* scriptSurface = ScriptTexture2D::toNative(surfaceObj);

				if (scriptSurface != nullptr)
				{
					HTexture textureHandle = scriptSurface->getTextureHandle();
					if (!textureHandle.isLoaded())
					{
						LOGERR("Render texture must be created using a fully loaded texture.");
					}
					else
						surfaceDesc.texture = textureHandle.getInternalPtr();
				}
			}

			RENDER_TEXTURE_DESC desc;
			desc.colorSurface = surfaceDesc;
			desc.depthStencilSurface = depthStencilSurfaceDesc;
			tex = RenderTexture::create(desc);
		}

		ScriptRenderTexture2D* scriptResource = new (bs_alloc<ScriptRenderTexture2D>()) ScriptRenderTexture2D(tex, isMulti, instance);
	}

	void ScriptRenderTexture2D::internal_getColorSurfaces(ScriptRenderTexture2D* thisPtr, MonoArray** value)
	{
		if (thisPtr->mIsMulti)
		{
			MultiRenderTexturePtr tex = thisPtr->getMultiRenderTexture();

			UINT32 numColorSurfaces = tex->getColorSurfaceCount();
			ScriptArray outArray = ScriptArray::create<ScriptTexture2D>(numColorSurfaces);

			for (UINT32 i = 0; i < numColorSurfaces; i++)
			{
				HTexture colorTex = tex->getBindableColorTexture(i);

				ScriptTexture2D* scriptSurface = ScriptResourceManager::instance().getScriptTexture(colorTex);
				if (scriptSurface == nullptr)
					scriptSurface = ScriptResourceManager::instance().createScriptTexture2D(colorTex);

				outArray.set(i, scriptSurface->getManagedInstance());
				*value = outArray.getInternal();
			}
		}
		else
		{
			RenderTexturePtr tex = thisPtr->getRenderTexture();
			ScriptArray outArray = ScriptArray::create<ScriptTexture2D>(1);

			HTexture colorTex = tex->getBindableColorTexture();

			ScriptTexture2D* scriptSurface = ScriptResourceManager::instance().getScriptTexture(colorTex);
			if (scriptSurface == nullptr)
				scriptSurface = ScriptResourceManager::instance().createScriptTexture2D(colorTex);

			outArray.set(0, scriptSurface->getManagedInstance());
			*value = outArray.getInternal();
		}
	}

	void ScriptRenderTexture2D::internal_getDepthStencilSurface(ScriptRenderTexture2D* thisPtr, MonoObject** value)
	{
		HTexture colorTex;
		if (thisPtr->mIsMulti)
		{
			MultiRenderTexturePtr tex = thisPtr->getMultiRenderTexture();
			colorTex = tex->getBindableDepthStencilTexture();
		}
		else
		{
			RenderTexturePtr tex = thisPtr->getRenderTexture();
			colorTex = tex->getBindableDepthStencilTexture();
		}

		ScriptTexture2D* scriptSurface = ScriptResourceManager::instance().getScriptTexture(colorTex);
		if (scriptSurface == nullptr)
			scriptSurface = ScriptResourceManager::instance().createScriptTexture2D(colorTex);

		*value = scriptSurface->getManagedInstance();
	}
}