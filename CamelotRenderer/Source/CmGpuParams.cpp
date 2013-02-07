#include "CmGpuParams.h"
#include "CmGpuParamDesc.h"
#include "CmGpuParamBlock.h"
#include "CmVector2.h"
#include "CmDebug.h"
#include "CmException.h"

namespace CamelotEngine
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

		mParamBlocks.resize(numParamBlockSlots);

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

	GpuParamBlockPtr GpuParams::getParamBlock(UINT32 slot) const
	{
		if(slot < 0 || slot >= (UINT32)mParamBlocks.size())
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mParamBlocks.size() - 1) + ". Requested: " + toString(slot));
		}

		return mParamBlocks[slot];
	}

	GpuParamBlockPtr GpuParams::getParamBlock(const String& name) const
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return nullptr;
		}

		return mParamBlocks[iterFind->second.slot];
	}

	void GpuParams::setParamBlock(UINT32 slot, GpuParamBlockPtr paramBlock)
	{
		if(slot < 0 || slot >= (UINT32)mParamBlocks.size())
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mParamBlocks.size() - 1) + ". Requested: " + toString(slot));
		}

		mParamBlocks[slot] = paramBlock;
	}

	void GpuParams::setParamBlock(const String& name, GpuParamBlockPtr paramBlock)
	{
		auto iterFind = mParamDesc.paramBlocks.find(name);

		if(iterFind == mParamDesc.paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return;
		}

		mParamBlocks[iterFind->second.slot] = paramBlock;
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

		GpuParamBlockPtr paramBlock = mParamBlocks[desc->paramBlockSlot];

		if(paramBlock == nullptr)
		{
			LOGWRN("Parameter exists but there is no ParamBlock set.");
			return;
		}

		paramBlock->write((desc->cpuMemOffset + arrayIndex * desc->elementSize) * sizeof(UINT32), value, sizeBytes);

		// Set unused bytes to 0
		if(sizeBytes < elementSizeBytes)
		{
			UINT32 diffSize = elementSizeBytes - sizeBytes;
			paramBlock->zeroOut((desc->cpuMemOffset + arrayIndex * desc->elementSize + sizeBytes)  * sizeof(UINT32), diffSize);
		}
	}

	void GpuParams::setTexture(const String& name, TextureHandle& val)
	{
		auto paramIter = mParamDesc.textures.find(name);
		if(paramIter == mParamDesc.textures.end())
		{
			LOGWRN("Texture with the name '" + name + "' doesn't exist.");
			return;
		}

		mTextures[paramIter->second.slot] = val;
	}

	TextureHandle GpuParams::getTexture(UINT32 slot)
	{
		if(slot < 0 || slot >= (UINT32)mTextures.size())
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mTextures.size() - 1) + ". Requested: " + toString(slot));
		}

		return mTextures[slot];
	}

	void GpuParams::setSamplerState(const String& name, SamplerStateHandle& val)
	{
		auto paramIter = mParamDesc.samplers.find(name);
		if(paramIter == mParamDesc.samplers.end())
		{
			LOGWRN("Sampler with the name '" + name + "' doesn't exist.");
			return;
		}

		mSamplerStates[paramIter->second.slot] = val;
	}

	SamplerStateHandle GpuParams::getSamplerState(UINT32 slot)
	{
		if(slot < 0 || slot >= (UINT32)mSamplerStates.size())
		{
			CM_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " + 
				toString(mSamplerStates.size() - 1) + ". Requested: " + toString(slot));
		}

		return mSamplerStates[slot];
	}

	void GpuParams::updateParamBuffers()
	{
		for(size_t i = 0; i < mParamBlocks.size(); i++)
		{
			if(mParamBlocks[i] != nullptr)
				mParamBlocks[i]->updateBuffer();
		}
	}

	GpuParamDataDesc* GpuParams::getParamDesc(const String& name) const
	{
		auto paramIter = mParamDesc.params.find(name);
		if(paramIter != mParamDesc.params.end())
			return &paramIter->second;

		return nullptr;
	}
}