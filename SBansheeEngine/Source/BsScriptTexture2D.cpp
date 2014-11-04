#include "BsScriptTexture2D.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsTexture.h"
#include "BsPixelUtil.h"
#include "BsException.h"
#include "BsScriptPixelData.h"
#include "BsScriptAsyncOp.h"
#include "BsCoreThread.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptTexture2D::ScriptTexture2D(MonoObject* instance, const HTexture& texture)
		:ScriptObject(instance), mTexture(texture)
	{

	}

	void ScriptTexture2D::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptTexture2D::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetPixels", &ScriptTexture2D::internal_getPixels);
		metaData.scriptClass->addInternalCall("Internal_GetGPUPixels", &ScriptTexture2D::internal_getGPUPixels);
		metaData.scriptClass->addInternalCall("Internal_SetPixels", &ScriptTexture2D::internal_setPixels);
	}

	void ScriptTexture2D::internal_createInstance(MonoObject* instance, PixelFormat format, UINT32 width,
		UINT32 height, TextureUsage usage, UINT32 numSamples, bool hasMipmaps, bool gammaCorrection)
	{
		int numMips = 0;
		if(hasMipmaps)
			numMips = PixelUtil::getMaxMipmaps(width, height, 1, format);

		HTexture texture = Texture::create(TEX_TYPE_2D, width, height, numMips, format, usage, gammaCorrection, numSamples);

		ScriptResourceManager::instance().createScriptTexture2D(instance, texture);
	}

	MonoObject* ScriptTexture2D::internal_getPixels(ScriptTexture2D* thisPtr, UINT32 mipLevel)
	{
		HTexture texture = thisPtr->mTexture;
		UINT32 subresourceIdx = texture->mapToSubresourceIdx(0, mipLevel);

		PixelDataPtr pixelData = thisPtr->mTexture->allocateSubresourceBuffer(subresourceIdx);

		thisPtr->mTexture->readDataSim(*pixelData, mipLevel);

		return ScriptPixelData::create(pixelData);
	}

	MonoObject* ScriptTexture2D::internal_getGPUPixels(ScriptTexture2D* thisPtr, UINT32 mipLevel)
	{
		HTexture texture = thisPtr->mTexture;
		UINT32 subresourceIdx = texture->mapToSubresourceIdx(0, mipLevel);

		PixelDataPtr readData = texture->allocateSubresourceBuffer(subresourceIdx);

		AsyncOp asyncOp = gCoreAccessor().readSubresource(texture.getInternalPtr(), subresourceIdx, readData);

		std::function<MonoObject*(const AsyncOp&, const PixelDataPtr&)> asyncOpToMono =
			[&](const AsyncOp& op, const PixelDataPtr& returnValue)
		{
			return ScriptPixelData::create(returnValue);
		};

		return ScriptAsyncOp::create(asyncOp, std::bind(asyncOpToMono, _1, readData));
	}

	void ScriptTexture2D::internal_setPixels(ScriptTexture2D* thisPtr, MonoObject* data, UINT32 mipLevel)
	{
		ScriptPixelData* scriptPixelData = ScriptPixelData::toNative(data);

		if (scriptPixelData != nullptr)
		{
			HTexture texture = thisPtr->mTexture;
			UINT32 subresourceIdx = texture->mapToSubresourceIdx(0, mipLevel);

			gCoreAccessor().writeSubresource(texture.getInternalPtr(), subresourceIdx, scriptPixelData->getInternalValue());
		}
	}

	void ScriptTexture2D::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;
		ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptTexture2D::setNativeHandle(const HResource& resource) 
	{ 
		mTexture = static_resource_cast<Texture>(resource);
	}
}