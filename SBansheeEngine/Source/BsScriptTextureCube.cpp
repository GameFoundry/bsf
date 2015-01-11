#include "BsScriptTextureCube.h"
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
	ScriptTextureCube::ScriptTextureCube(MonoObject* instance, const HTexture& texture)
		:ScriptObject(instance), mTexture(texture)
	{

	}

	void ScriptTextureCube::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptTextureCube::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetPixels", &ScriptTextureCube::internal_getPixels);
		metaData.scriptClass->addInternalCall("Internal_GetGPUPixels", &ScriptTextureCube::internal_getGPUPixels);
		metaData.scriptClass->addInternalCall("Internal_SetPixels", &ScriptTextureCube::internal_setPixels);
	}

	void ScriptTextureCube::internal_createInstance(MonoObject* instance, PixelFormat format, UINT32 width,
		UINT32 height, TextureUsage usage, UINT32 numSamples, bool hasMipmaps, bool gammaCorrection)
	{
		int numMips = 0;
		if (hasMipmaps)
			numMips = PixelUtil::getMaxMipmaps(width, height, 1, format);

		HTexture texture = Texture::create(TEX_TYPE_CUBE_MAP, width, height, numMips, format, usage, gammaCorrection, numSamples);

		ScriptResourceManager::instance().createScriptTextureCube(instance, texture);
	}

	MonoObject* ScriptTextureCube::internal_getPixels(ScriptTextureCube* thisPtr, UINT32 face, UINT32 mipLevel)
	{
		HTexture texture = thisPtr->mTexture;
		UINT32 subresourceIdx = texture->getProperties().mapToSubresourceIdx(face, mipLevel);

		PixelDataPtr pixelData = thisPtr->mTexture->getProperties().allocateSubresourceBuffer(subresourceIdx);

		thisPtr->mTexture->readData(*pixelData, mipLevel, face);

		return ScriptPixelData::create(pixelData);
	}

	MonoObject* ScriptTextureCube::internal_getGPUPixels(ScriptTextureCube* thisPtr, UINT32 face, UINT32 mipLevel)
	{
		HTexture texture = thisPtr->mTexture;
		UINT32 subresourceIdx = texture->getProperties().mapToSubresourceIdx(face, mipLevel);

		PixelDataPtr readData = texture->getProperties().allocateSubresourceBuffer(subresourceIdx);
		AsyncOp asyncOp = texture->readSubresource(gCoreAccessor(), subresourceIdx, readData);

		std::function<MonoObject*(const AsyncOp&, const PixelDataPtr&)> asyncOpToMono =
			[&](const AsyncOp& op, const PixelDataPtr& returnValue)
		{
			return ScriptPixelData::create(returnValue);
		};

		return ScriptAsyncOp::create(asyncOp, std::bind(asyncOpToMono, _1, readData));
	}

	void ScriptTextureCube::internal_setPixels(ScriptTextureCube* thisPtr, MonoObject* data, UINT32 face, UINT32 mipLevel)
	{
		ScriptPixelData* scriptPixelData = ScriptPixelData::toNative(data);

		if (scriptPixelData != nullptr)
		{
			HTexture texture = thisPtr->mTexture;
			UINT32 subresourceIdx = texture->getProperties().mapToSubresourceIdx(face, mipLevel);

			texture->writeSubresource(gCoreAccessor(), subresourceIdx, scriptPixelData->getInternalValue(), false);
		}
	}

	void ScriptTextureCube::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptTextureCube::setNativeHandle(const HResource& resource)
	{
		mTexture = static_resource_cast<Texture>(resource);
	}
}