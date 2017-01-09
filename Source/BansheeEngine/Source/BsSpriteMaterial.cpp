//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsSprite.h"
#include "BsMaterial.h"
#include "BsTexture.h"
#include "BsMesh.h"
#include "BsShader.h"
#include "BsRendererUtility.h"
#include "BsGpuParamsSet.h"
#include "BsCoreThread.h"

namespace bs
{
	SpriteMaterial::SpriteMaterial(UINT32 id, const HMaterial& material)
		:mId(id), mMaterialStored(false)
	{
		mMaterial = material->getCore();
		mMaterialStored.store(true, std::memory_order_release);

		gCoreThread().queueCommand(std::bind(&SpriteMaterial::initialize, this));
	}

	SpriteMaterial::~SpriteMaterial()
	{
		gCoreThread().queueCommand(std::bind(&SpriteMaterial::destroy, mMaterial, mParams));
	}

	void SpriteMaterial::initialize()
	{
		// Make sure that mMaterial assignment completes on the previous thread before continuing
		bool materialStored = mMaterialStored.load(std::memory_order_acquire);
		assert(materialStored == true);

		mParams = mMaterial->createParamsSet();

		SPtr<ct::ShaderCore> shader = mMaterial->getShader();
		if(shader->hasTextureParam("gMainTexture"))
		{
			mTextureParam = mMaterial->getParamTexture("gMainTexture");
			mSamplerParam = mMaterial->getParamSamplerState("gMainTexSamp");
		}

		static StringID GUIParamsSemantic = "GUIParams";
		const Map<String, SHADER_PARAM_BLOCK_DESC>& paramBlockDescs = shader->getParamBlocks();

		for (auto& paramBlockDesc : paramBlockDescs)
		{
			if (paramBlockDesc.second.rendererSemantic == GUIParamsSemantic)
				mParamBufferIdx = mParams->getParamBlockBufferIndex(paramBlockDesc.second.name);
		}

		if(mParamBufferIdx == -1)
			LOGERR("Sprite material shader missing \"GUIParams\" block.");
	}

	void SpriteMaterial::destroy(const SPtr<ct::MaterialCore>& material, const SPtr<ct::GpuParamsSetCore>& params)
	{
		// Do nothing, we just need to make sure the material pointer's last reference is lost while on the core thread
	}

	UINT64 SpriteMaterial::getMergeHash(const SpriteMaterialInfo& info) const
	{
		UINT64 textureId = 0;
		if (info.texture.isLoaded())
			textureId = info.texture->getInternalID();

		size_t hash = 0;
		hash_combine(hash, info.groupId);
		hash_combine(hash, getId());
		hash_combine(hash, textureId);
		hash_combine(hash, info.tint);

		return (UINT64)hash;
	}

	void SpriteMaterial::render(const SPtr<ct::MeshCoreBase>& mesh, const SPtr<ct::TextureCore>& texture,
		const SPtr<ct::SamplerStateCore>& sampler, const SPtr<ct::GpuParamBlockBufferCore>& paramBuffer,
		const SPtr<SpriteMaterialExtraInfo>& additionalData) const
	{
		SPtr<ct::TextureCore> spriteTexture;
		if (texture != nullptr)
			spriteTexture = texture;
		else
			spriteTexture = ct::TextureCore::WHITE;

		mTextureParam.set(spriteTexture);
		mSamplerParam.set(sampler);

		if(mParamBufferIdx != -1)
			mParams->setParamBlockBuffer(mParamBufferIdx, paramBuffer, true);

		mMaterial->updateParamsSet(mParams);

		ct::gRendererUtility().setPass(mMaterial);
		ct::gRendererUtility().setPassParams(mParams);
		ct::gRendererUtility().draw(mesh, mesh->getProperties().getSubMesh(0));
	}
}