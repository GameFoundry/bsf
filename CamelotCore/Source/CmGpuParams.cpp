#include "CmGpuParams.h"
#include "CmGpuParamDesc.h"
#include "CmGpuParamBlock.h"
#include "CmGpuParamBlockBuffer.h"
#include "CmVector2.h"
#include "CmDebug.h"
#include "CmException.h"

namespace CamelotFramework
{
	GpuParams::GpuParams(GpuParamDesc& paramDesc)
		:mParamDesc(paramDesc), mTransposeMatrices(false)
	{
		UINT32 numParamBlockSlots = 0;
		for(auto iter = mParamDesc.paramBlocks.begin(); iter != mParamDesc.paramBlocks.end(); ++iter)
		{
			if((iter->second.slot + 1) > numParamBlockSlots)
				numParamBlockSlots = iter->second.slot + 1;
		}

		mParamBlocks.resize(numParamBlockSlots, nullptr);
		mParamBlockBuffers.resize(numParamBlockSlots);

		UINT32 numTextureSlots = 0;
		for(auto iter = mParamDesc.textures.begin(); iter != mParamDesc.textures.end(); ++iter)
		{
			if((iter->second.slot + 1) > numTextureSlots)
				numTextureSlots = iter->second.slot + 1;
		}

		mTextures.resize(numTextureSlots);

		UINT32 numSamplerSlots = 0;
		for(auto iter = mParamDesc.samplers.begin(); iter != mParamDesc.samplers.end(); ++iter)
		{
			if((iter->second.slot + 1) > numSamplerSlots)
				numSamplerSlots = iter->second.slot + 1;
		}

		mSamplerStates.resize(numSamplerSlots);
	}

	GpuParams::~GpuParams()
	{
		for(auto& paramBlock : mParamBlocks)
		{
			cm_delete<PoolAlloc>(paramBlock);
		}
	}

	GpuParamBlockBufferPtr GpuParams::getParamBlockBuffer(UINT32 slot) const
	{
		if(slot < 0 || slot >= (UINT32)mParamBlocks.size())
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString((int)mParamBlocks.size() - 1) + ". Requested: " + toString(slot));
		}

		return mParamBlockBuffers[slot];
	}

	GpuParamBlockBufferPtr GpuParams::getParamBlockBuffer(const String& name) const
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return nullptr;
		}

		return mParamBlockBuffers[iterFind->second.slot];
	}

	void GpuParams::setParamBlockBuffer(UINT32 slot, GpuParamBlockBufferPtr paramBlockBuffer)
	{
		if(slot < 0 || slot >= (UINT32)mParamBlocks.size())
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString((int)mParamBlocks.size() - 1) + ". Requested: " + toString(slot));
		}

		if(mParamBlocks[slot] != nullptr)
			cm_delete<PoolAlloc>(mParamBlocks[slot]);

		mParamBlocks[slot] = cm_new<GpuParamBlock, PoolAlloc>(paramBlockBuffer->getSize());
		mParamBlockBuffers[slot] = paramBlockBuffer;
	}

	void GpuParams::setParamBlockBuffer(const String& name, GpuParamBlockBufferPtr paramBlockBuffer)
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return;
		}

		if(mParamBlocks[iterFind->second.slot] != nullptr)
			cm_delete<PoolAlloc>(mParamBlocks[iterFind->second.slot]);

		mParamBlocks[iterFind->second.slot] = cm_new<GpuParamBlock, PoolAlloc>(paramBlockBuffer->getSize());
		mParamBlockBuffers[iterFind->second.slot] = paramBlockBuffer;
	}

	UINT32 GpuParams::getDataParamSize(const String& name) const
	{
		GpuParamDataDesc* desc = getParamDesc(name);
		if(desc != nullptr)
			return desc->elementSize * 4;

		return 0;
	}

	bool GpuParams::hasParam(const String& name) const
	{
		return getParamDesc(name) != nullptr;
	}

	bool GpuParams::hasTexture(const String& name) const
	{
		auto paramIter = mParamDesc.textures.find(name);
		if(paramIter != mParamDesc.textures.end())
			return true;

		return false;
	}

	bool GpuParams::hasSamplerState(const String& name) const
	{
		auto paramIter = mParamDesc.samplers.find(name);
		if(paramIter != mParamDesc.samplers.end())
			return true;

		return false;
	}

	bool GpuParams::hasParamBlock(const String& name) const
	{
		auto paramBlockIter = mParamDesc.paramBlocks.find(name);
		if(paramBlockIter != mParamDesc.paramBlocks.end())
			return true;

		return false;
	}

	void GpuParams::setParam(const String& name, float value, UINT32 arrayIndex)
	{
		setParam(name, (void*)&value, 1 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, int value, UINT32 arrayIndex)
	{
		setParam(name, (void*)&value, 1 * sizeof(int), arrayIndex);
	}

	void GpuParams::setParam(const String& name, bool value, UINT32 arrayIndex)
	{
		setParam(name, (void*)&value, 1 * sizeof(bool), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const Vector4& vec, UINT32 arrayIndex)
	{
		setParam(name, (void*)&vec, 4 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const Vector3& vec, UINT32 arrayIndex)
	{
		setParam(name, (void*)&vec, 3 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const Vector2& vec, UINT32 arrayIndex)
	{
		setParam(name, (void*)&vec, 2 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const Matrix4& mat, UINT32 arrayIndex)
	{
		if (mTransposeMatrices)
		{
			Matrix4 transMat = mat.transpose();
			setParam(name, (void*)&transMat, 16 * sizeof(float), arrayIndex);
		}
		else
		{
			setParam(name, (void*)&mat, 16 * sizeof(float), arrayIndex);
		}
	}

	void GpuParams::setParam(const String& name, const Matrix3& mat, UINT32 arrayIndex)
	{
		if (mTransposeMatrices)
		{
			Matrix3 transMat = mat.transpose();
			setParam(name, (void*)&transMat, 9 * sizeof(float), arrayIndex);
		}
		else
		{
			setParam(name, (void*)&mat, 9 * sizeof(float), arrayIndex);
		}
	}

	void GpuParams::setParam(const String& name, const Color& color, UINT32 arrayIndex)
	{
		setParam(name, (void*)&color, 4 * sizeof(float), arrayIndex);
	}

	void GpuParams::setParam(const String& name, const void* value, UINT32 sizeBytes, UINT32 arrayIndex)
	{
		GpuParamDataDesc* desc = getParamDesc(name);

		if(desc == nullptr)
		{
			LOGWRN("Cannot find parameter with the name '" + name + "'");
			return;
		}

		if(arrayIndex >= desc->arraySize)
		{
			CM_EXCEPT(InvalidParametersException, "Array index out of range. Array size: " + 
				toString(desc->arraySize) + ". Requested size: " + toString(arrayIndex));
		}

		UINT32 elementSizeBytes = desc->elementSize * sizeof(UINT32);
		if(sizeBytes > elementSizeBytes)
		{
			CM_EXCEPT(InvalidParametersException, "Provided element size larger than maximum element size. Maximum size: " + 
				toString(elementSizeBytes) + ". Supplied size: " + toString(sizeBytes));
		}

		GpuParamBlock* paramBlock = mParamBlocks[desc->paramBlockSlot];

		if(paramBlock == nullptr)
		{
			LOGWRN("Parameter exists but there is no ParamBlock set.");
			return;
		}

		paramBlock->write((desc->cpuMemOffset + arrayIndex * desc->arrayElementStride) * sizeof(UINT32), value, sizeBytes);

		// Set unused bytes to 0
		if(sizeBytes < elementSizeBytes)
		{
			UINT32 diffSize = elementSizeBytes - sizeBytes;
			paramBlock->zeroOut((desc->cpuMemOffset + arrayIndex * desc->arrayElementStride)  * sizeof(UINT32) + sizeBytes, diffSize);
		}
	}

	void GpuParams::setTexture(const String& name, const HTexture& val)
	{
		auto paramIter = mParamDesc.textures.find(name);
		if(paramIter == mParamDesc.textures.end())
		{
			LOGWRN("Texture with the name '" + name + "' doesn't exist.");
			return;
		}

		mTextures[paramIter->second.slot] = val;
	}

	HTexture GpuParams::getTexture(UINT32 slot)
	{
		if(slot < 0 || slot >= (UINT32)mTextures.size())
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString((int)mTextures.size() - 1) + ". Requested: " + toString(slot));
		}

		return mTextures[slot];
	}

	void GpuParams::setSamplerState(const String& name, const HSamplerState& val)
	{
		auto paramIter = mParamDesc.samplers.find(name);
		if(paramIter == mParamDesc.samplers.end())
		{
			LOGWRN("Sampler with the name '" + name + "' doesn't exist.");
			return;
		}

		mSamplerStates[paramIter->second.slot] = val;
	}

	HSamplerState GpuParams::getSamplerState(UINT32 slot)
	{
		if(slot < 0 || slot >= (UINT32)mSamplerStates.size())
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString((int)mSamplerStates.size() - 1) + ". Requested: " + toString(slot));
		}

		return mSamplerStates[slot];
	}

	GpuParamDataDesc* GpuParams::getParamDesc(const String& name) const
	{
		auto paramIter = mParamDesc.params.find(name);
		if(paramIter != mParamDesc.params.end())
			return &paramIter->second;

		return nullptr;
	}

	BindableGpuParams GpuParams::createBindableCopy(GpuParamsPtr params)
	{
		GpuParams* copy = cm_new<GpuParams, ScratchAlloc>(std::ref(params->mParamDesc));

		copy->mTransposeMatrices = params->mTransposeMatrices;
		copy->mSamplerStates = params->mSamplerStates;
		copy->mTextures = params->mTextures;
		copy->mParamBlockBuffers =params-> mParamBlockBuffers;

		copy->mParamBlocks.clear();
		for(auto& paramBlock : params->mParamBlocks)
		{
			GpuParamBlock* blockCopy = cm_new<GpuParamBlock, ScratchAlloc>(paramBlock);

			copy->mParamBlocks.push_back(blockCopy);
		}

		return BindableGpuParams(copy);
	}

	void GpuParams::updateHardwareBuffers()
	{
		for(size_t i = 0; i < mParamBlocks.size(); i++)
		{
			if(mParamBlocks[i] != nullptr && mParamBlockBuffers[i] != nullptr)
			{
				if(mParamBlocks[i]->isDirty())
					mParamBlocks[i]->uploadToBuffer(mParamBlockBuffers[i]);
			}
		}
	}

	void GpuParams::releaseBindableCopy(BindableGpuParams& bindableParams)
	{
		cm_delete<ScratchAlloc>(bindableParams.mParams);
	}

	BindableGpuParams::BindableGpuParams(GpuParams* params)
		:mParams(params), mIsDataOwner(true)
	{

	}

	BindableGpuParams::~BindableGpuParams()
	{
		if(mIsDataOwner)
			GpuParams::releaseBindableCopy(*this);
	}

	BindableGpuParams::BindableGpuParams(const BindableGpuParams& source)
	{
		mParams = source.mParams;

		mIsDataOwner = true;
		source.mIsDataOwner = false;
	}
}