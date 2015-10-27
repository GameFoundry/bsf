#include "BsGpuParams.h"
#include "BsGpuParamDesc.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsVector2.h"
#include "BsTexture.h"
#include "BsSamplerState.h"
#include "BsFrameAlloc.h"
#include "BsDebug.h"
#include "BsException.h"
#include "BsVectorNI.h"
#include "BsMatrixNxM.h"

namespace BansheeEngine
{
	GpuParamsBase::GpuParamsBase(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
		:mParamDesc(paramDesc), mTransposeMatrices(transposeMatrices), mNumParamBlocks(0), mNumSamplerStates(0),
		mNumTextures(0), mTextureInfo(nullptr)
	{
		for (auto& paramBlock : mParamDesc->paramBlocks)
		{
			if ((paramBlock.second.slot + 1) > mNumParamBlocks)
				mNumParamBlocks = paramBlock.second.slot + 1;
		}

		for (auto& texture : mParamDesc->textures)
		{
			if ((texture.second.slot + 1) > mNumTextures)
				mNumTextures = texture.second.slot + 1;
		}

		for (auto& sampler : mParamDesc->samplers)
		{
			if ((sampler.second.slot + 1) > mNumSamplerStates)
				mNumSamplerStates = sampler.second.slot + 1;
		}

		mTextureInfo = bs_newN<BoundTextureInfo>(mNumTextures);
	}

	GpuParamsBase::~GpuParamsBase()
	{
		bs_deleteN(mTextureInfo, mNumTextures);
	}

UINT32 GpuParamsBase::getDataParamSize(const String& name) const
	{
		GpuParamDataDesc* desc = getParamDesc(name);
		if(desc != nullptr)
			return desc->elementSize * 4;

		return 0;
	}

	bool GpuParamsBase::hasParam(const String& name) const
	{
		return getParamDesc(name) != nullptr;
	}

	bool GpuParamsBase::hasTexture(const String& name) const
	{
		auto paramIter = mParamDesc->textures.find(name);
		if(paramIter != mParamDesc->textures.end())
			return true;

		return false;
	}

	bool GpuParamsBase::hasSamplerState(const String& name) const
	{
		auto paramIter = mParamDesc->samplers.find(name);
		if(paramIter != mParamDesc->samplers.end())
			return true;

		return false;
	}

	bool GpuParamsBase::hasParamBlock(const String& name) const
	{
		auto paramBlockIter = mParamDesc->paramBlocks.find(name);
		if(paramBlockIter != mParamDesc->paramBlocks.end())
			return true;

		return false;
	}

	GpuParamDataDesc* GpuParamsBase::getParamDesc(const String& name) const
	{
		auto paramIter = mParamDesc->params.find(name);
		if (paramIter != mParamDesc->params.end())
			return &paramIter->second;

		return nullptr;
	}

	bool GpuParamsBase::isLoadStoreTexture(UINT32 slot) const
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mTextureInfo[slot].isLoadStore;
	}

	void GpuParamsBase::setIsLoadStoreTexture(UINT32 slot, bool isLoadStore)
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		mTextureInfo[slot].isLoadStore = isLoadStore;
	}

	const TextureSurface& GpuParamsBase::getLoadStoreSurface(UINT32 slot) const
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mTextureInfo[slot].surface;
	}

	void GpuParamsBase::setLoadStoreSurface(UINT32 slot, const TextureSurface& surface) const
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		mTextureInfo[slot].surface = surface;
	}

	template<bool Core>
	TGpuParams<Core>::TGpuParams(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
		:GpuParamsBase(paramDesc, transposeMatrices), mParamBlockBuffers(nullptr), mTextures(nullptr),
		mSamplerStates(nullptr)
	{
		if (mNumParamBlocks > 0)
			mParamBlockBuffers = bs_newN<ParamsBufferType>(mNumParamBlocks);

		if (mNumTextures > 0)
			mTextures = bs_newN<TextureType>(mNumTextures);

		if (mNumSamplerStates > 0)
			mSamplerStates = bs_newN<SamplerType>(mNumSamplerStates);
	}

	template<bool Core>
	TGpuParams<Core>::~TGpuParams()
	{
		if (mParamBlockBuffers != nullptr)
			bs_deleteN(mParamBlockBuffers, mNumParamBlocks);

		if (mTextures != nullptr)
			bs_deleteN(mTextures, mNumTextures);

		if (mSamplerStates != nullptr)
			bs_deleteN(mSamplerStates, mNumSamplerStates);
	}

	template<bool Core>
	void TGpuParams<Core>::setParamBlockBuffer(UINT32 slot, const ParamsBufferType& paramBlockBuffer)
	{
		if (slot < 0 || slot >= mNumParamBlocks)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumParamBlocks - 1) + ". Requested: " + toString(slot));
		}

		mParamBlockBuffers[slot] = paramBlockBuffer;

		_markCoreDirty();
	}

	template<bool Core>
	void TGpuParams<Core>::setParamBlockBuffer(const String& name, const ParamsBufferType& paramBlockBuffer)
	{
		auto iterFind = mParamDesc->paramBlocks.find(name);

		if (iterFind == mParamDesc->paramBlocks.end())
		{
			LOGWRN("Cannot find parameter block with the name: " + name);
			return;
		}

		mParamBlockBuffers[iterFind->second.slot] = paramBlockBuffer;

		_markCoreDirty();
	}

	template<class T> struct TDataParamInfo { };
	template<> struct TDataParamInfo < float > { enum { TypeId = GPDT_FLOAT1 }; };
	template<> struct TDataParamInfo < Color > { enum { TypeId = GPDT_FLOAT4 }; };
	template<> struct TDataParamInfo < Vector2 > { enum { TypeId = GPDT_FLOAT2 }; };
	template<> struct TDataParamInfo < Vector3 > { enum { TypeId = GPDT_FLOAT3 }; };
	template<> struct TDataParamInfo < Vector4 > { enum { TypeId = GPDT_FLOAT4 }; };
	template<> struct TDataParamInfo < Matrix3 > { enum { TypeId = GPDT_MATRIX_3X3 }; };
	template<> struct TDataParamInfo < Matrix4 > { enum { TypeId = GPDT_MATRIX_4X4 }; };

	template<bool Core>
	template<class T> 
	void TGpuParams<Core>::getParam(const String& name, TGpuDataParam<T, Core>& output) const
	{
		auto iterFind = mParamDesc->params.find(name);

		if (iterFind == mParamDesc->params.end())
		{
			output = TGpuDataParam<T, Core>(&iterFind->second, nullptr);
			LOGWRN("Cannot find parameter with the name '" + name + "'");
		}
		else
			output = TGpuDataParam<T, Core>(&iterFind->second, _getThisPtr());
	}

	template<bool Core>
	void TGpuParams<Core>::getStructParam(const String& name, TGpuParamStruct<Core>& output) const
	{
		auto iterFind = mParamDesc->params.find(name);

		if (iterFind == mParamDesc->params.end() || iterFind->second.type != GPDT_STRUCT)
		{
			output = TGpuParamStruct<Core>(&iterFind->second, nullptr);
			LOGWRN("Cannot find struct parameter with the name '" + name + "'");
		}
		else
			output = TGpuParamStruct<Core>(&iterFind->second, _getThisPtr());
	}

	template<bool Core>
	void TGpuParams<Core>::getTextureParam(const String& name, TGpuParamTexture<Core>& output) const
	{
		auto iterFind = mParamDesc->textures.find(name);

		if (iterFind == mParamDesc->textures.end())
		{
			output = TGpuParamTexture<Core>(&iterFind->second, nullptr);
			LOGWRN("Cannot find texture parameter with the name '" + name + "'");
		}
		else
			output = TGpuParamTexture<Core>(&iterFind->second, _getThisPtr());
	}

	template<bool Core>
	void TGpuParams<Core>::getLoadStoreTextureParam(const String& name, TGpuParamLoadStoreTexture<Core>& output) const
	{
		auto iterFind = mParamDesc->textures.find(name);

		if (iterFind == mParamDesc->textures.end())
		{
			output = TGpuParamLoadStoreTexture<Core>(&iterFind->second, nullptr);
			LOGWRN("Cannot find texture parameter with the name '" + name + "'");
		}
		else
			output = TGpuParamLoadStoreTexture<Core>(&iterFind->second, _getThisPtr());
	}

	template<bool Core>
	void TGpuParams<Core>::getSamplerStateParam(const String& name, TGpuParamSampState<Core>& output) const
	{
		auto iterFind = mParamDesc->samplers.find(name);

		if (iterFind == mParamDesc->samplers.end())
		{
			output = TGpuParamSampState<Core>(&iterFind->second, nullptr);
			LOGWRN("Cannot find sampler state parameter with the name '" + name + "'");
		}
		else
			output = TGpuParamSampState<Core>(&iterFind->second, _getThisPtr());
	}

	template<bool Core>
	typename TGpuParams<Core>::ParamsBufferType TGpuParams<Core>::getParamBlockBuffer(UINT32 slot) const
	{
		if (slot < 0 || slot >= mNumParamBlocks)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumParamBlocks - 1) + ". Requested: " + toString(slot));
		}

		return mParamBlockBuffers[slot];
	}

	template<bool Core>
	typename TGpuParams<Core>::TextureType TGpuParams<Core>::getTexture(UINT32 slot)
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		return mTextures[slot];
	}

	template<bool Core>
	typename TGpuParams<Core>::SamplerType TGpuParams<Core>::getSamplerState(UINT32 slot)
	{
		if (slot < 0 || slot >= mNumSamplerStates)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumSamplerStates - 1) + ". Requested: " + toString(slot));
		}

		return mSamplerStates[slot];
	}

	template<bool Core>
	void TGpuParams<Core>::setTexture(UINT32 slot, const TextureType& texture)
	{
		if (slot < 0 || slot >= mNumTextures)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumTextures - 1) + ". Requested: " + toString(slot));
		}

		mTextures[slot] = texture;

		_markResourcesDirty();
		_markCoreDirty();
	}

	template<bool Core>
	void TGpuParams<Core>::setSamplerState(UINT32 slot, const SamplerType& sampler)
	{
		if (slot < 0 || slot >= mNumSamplerStates)
		{
			BS_EXCEPT(InvalidParametersException, "Index out of range: Valid range: 0 .. " +
				toString(mNumSamplerStates - 1) + ". Requested: " + toString(slot));
		}

		mSamplerStates[slot] = sampler;

		_markResourcesDirty();
		_markCoreDirty();
	}

	template class TGpuParams < false > ;
	template class TGpuParams < true > ;

	template BS_CORE_EXPORT void TGpuParams<false>::getParam<float>(const String&, TGpuDataParam<float, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<int>(const String&, TGpuDataParam<int, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Color>(const String&, TGpuDataParam<Color, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Vector2>(const String&, TGpuDataParam<Vector2, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Vector3>(const String&, TGpuDataParam<Vector3, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Vector4>(const String&, TGpuDataParam<Vector4, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Vector2I>(const String&, TGpuDataParam<Vector2I, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Vector3I>(const String&, TGpuDataParam<Vector3I, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Vector4I>(const String&, TGpuDataParam<Vector4I, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix2>(const String&, TGpuDataParam<Matrix2, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix2x3>(const String&, TGpuDataParam<Matrix2x3, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix2x4>(const String&, TGpuDataParam<Matrix2x4, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix3>(const String&, TGpuDataParam<Matrix3, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix3x2>(const String&, TGpuDataParam<Matrix3x2, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix3x4>(const String&, TGpuDataParam<Matrix3x4, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix4>(const String&, TGpuDataParam<Matrix4, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix4x2>(const String&, TGpuDataParam<Matrix4x2, false>&) const;
	template BS_CORE_EXPORT void TGpuParams<false>::getParam<Matrix4x3>(const String&, TGpuDataParam<Matrix4x3, false>&) const;

	template BS_CORE_EXPORT void TGpuParams<true>::getParam<float>(const String&, TGpuDataParam<float, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<int>(const String&, TGpuDataParam<int, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Color>(const String&, TGpuDataParam<Color, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Vector2>(const String&, TGpuDataParam<Vector2, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Vector3>(const String&, TGpuDataParam<Vector3, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Vector4>(const String&, TGpuDataParam<Vector4, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Vector2I>(const String&, TGpuDataParam<Vector2I, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Vector3I>(const String&, TGpuDataParam<Vector3I, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Vector4I>(const String&, TGpuDataParam<Vector4I, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix2>(const String&, TGpuDataParam<Matrix2, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix2x3>(const String&, TGpuDataParam<Matrix2x3, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix2x4>(const String&, TGpuDataParam<Matrix2x4, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix3>(const String&, TGpuDataParam<Matrix3, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix3x2>(const String&, TGpuDataParam<Matrix3x2, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix3x4>(const String&, TGpuDataParam<Matrix3x4, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix4>(const String&, TGpuDataParam<Matrix4, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix4x2>(const String&, TGpuDataParam<Matrix4x2, true>&) const;
	template BS_CORE_EXPORT void TGpuParams<true>::getParam<Matrix4x3>(const String&, TGpuDataParam<Matrix4x3, true>&) const;

	GpuParamsCore::GpuParamsCore(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
		: TGpuParams(paramDesc, transposeMatrices)
	{

	}

	SPtr<GpuParamsCore> GpuParamsCore::_getThisPtr() const
	{
		return std::static_pointer_cast<GpuParamsCore>(getThisPtr());
	}

	void GpuParamsCore::updateHardwareBuffers()
	{
		for (UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			if (mParamBlockBuffers[i] != nullptr)
			{
				mParamBlockBuffers[i]->flushToGPU();
			}
		}
	}

	void GpuParamsCore::syncToCore(const CoreSyncData& data)
	{
		UINT32 textureInfoSize = mNumTextures * sizeof(BoundTextureInfo);
		UINT32 paramBufferSize = mNumParamBlocks * sizeof(SPtr<GpuParamBlockBufferCore>);
		UINT32 textureArraySize = mNumTextures * sizeof(SPtr<TextureCore>);
		UINT32 samplerArraySize = mNumSamplerStates * sizeof(SPtr<SamplerStateCore>);

		UINT32 totalSize = textureInfoSize + paramBufferSize + textureArraySize + samplerArraySize;

		UINT32 textureInfoOffset = 0;
		UINT32 paramBufferOffset = textureInfoOffset + textureInfoSize;
		UINT32 textureArrayOffset = paramBufferOffset + paramBufferSize;
		UINT32 samplerArrayOffset = textureArrayOffset + textureArraySize;

		assert(data.getBufferSize() == totalSize);

		UINT8* dataPtr = data.getBuffer();

		BoundTextureInfo* textureInfos = (BoundTextureInfo*)(dataPtr + textureInfoOffset);
		SPtr<GpuParamBlockBufferCore>* paramBuffers = (SPtr<GpuParamBlockBufferCore>*)(dataPtr + paramBufferOffset);
		SPtr<TextureCore>* textures = (SPtr<TextureCore>*)(dataPtr + textureArrayOffset);
		SPtr<SamplerStateCore>* samplers = (SPtr<SamplerStateCore>*)(dataPtr + samplerArrayOffset);

		// Copy & destruct
		for (UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			mParamBlockBuffers[i] = paramBuffers[i];
			paramBuffers[i].~SPtr<GpuParamBlockBufferCore>();
		}

		for (UINT32 i = 0; i < mNumTextures; i++)
		{
			mTextureInfo[i] = textureInfos[i];
			textureInfos[i].~BoundTextureInfo();

			mTextures[i] = textures[i];
			textures[i].~SPtr<TextureCore>();
		}

		for (UINT32 i = 0; i < mNumSamplerStates; i++)
		{
			mSamplerStates[i] = samplers[i];
			samplers[i].~SPtr<SamplerStateCore>();
		}
	}

	SPtr<GpuParamsCore> GpuParamsCore::create(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
	{
		GpuParamsCore* params = new (bs_alloc<GpuParamsCore>()) GpuParamsCore(paramDesc, transposeMatrices);
		SPtr<GpuParamsCore> paramsPtr = bs_shared_ptr<GpuParamsCore>(params);
		paramsPtr->_setThisPtr(paramsPtr);

		return paramsPtr;
	}

	GpuParams::GpuParams(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
		: TGpuParams(paramDesc, transposeMatrices)
	{

	}

	SPtr<GpuParams> GpuParams::_getThisPtr() const
	{
		return std::static_pointer_cast<GpuParams>(getThisPtr());
	}

	SPtr<GpuParamsCore> GpuParams::getCore() const
	{
		return std::static_pointer_cast<GpuParamsCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> GpuParams::createCore() const
	{
		GpuParamsCore* obj = new (bs_alloc<GpuParamsCore>()) GpuParamsCore(mParamDesc, mTransposeMatrices);

		SPtr<CoreObjectCore> coreObj = bs_shared_ptr<GpuParamsCore>(obj);
		coreObj->_setThisPtr(coreObj);

		return coreObj;
	}

	void GpuParams::_markCoreDirty()
	{
		markCoreDirty();
	}

	void GpuParams::_markResourcesDirty()
	{
		markListenerResourcesDirty();
	}

	SPtr<GpuParams> GpuParams::create(const GpuParamDescPtr& paramDesc, bool transposeMatrices)
	{
		GpuParams* params = new (bs_alloc<GpuParams>()) GpuParams(paramDesc, transposeMatrices);
		SPtr<GpuParams> paramsPtr = bs_core_ptr<GpuParams>(params);
		paramsPtr->_setThisPtr(paramsPtr);
		paramsPtr->initialize();
		
		return paramsPtr;
	}

	CoreSyncData GpuParams::syncToCore(FrameAlloc* allocator)
	{
		UINT32 textureInfoSize = mNumTextures * sizeof(BoundTextureInfo);
		UINT32 paramBufferSize = mNumParamBlocks * sizeof(SPtr<GpuParamBlockBufferCore>);
		UINT32 textureArraySize = mNumTextures * sizeof(SPtr<TextureCore>);
		UINT32 samplerArraySize = mNumSamplerStates * sizeof(SPtr<SamplerStateCore>);

		UINT32 totalSize = textureInfoSize + paramBufferSize + textureArraySize + samplerArraySize;

		UINT32 textureInfoOffset = 0;
		UINT32 paramBufferOffset = textureInfoOffset + textureInfoSize;
		UINT32 textureArrayOffset = paramBufferOffset + paramBufferSize;
		UINT32 samplerArrayOffset = textureArrayOffset + textureArraySize;

		UINT8* data = allocator->alloc(totalSize);

		BoundTextureInfo* textureInfos = (BoundTextureInfo*)(data + textureInfoOffset);
		SPtr<GpuParamBlockBufferCore>* paramBuffers = (SPtr<GpuParamBlockBufferCore>*)(data + paramBufferOffset);
		SPtr<TextureCore>* textures = (SPtr<TextureCore>*)(data + textureArrayOffset);
		SPtr<SamplerStateCore>* samplers = (SPtr<SamplerStateCore>*)(data + samplerArrayOffset);

		// Construct & copy
		for (UINT32 i = 0; i < mNumParamBlocks; i++)
		{
			new (&paramBuffers[i]) SPtr<GpuParamBlockBufferCore>();

			if (mParamBlockBuffers[i] != nullptr)
				paramBuffers[i] = mParamBlockBuffers[i]->getCore();
		}

		for (UINT32 i = 0; i < mNumTextures; i++)
		{
			new (&textureInfos[i]) BoundTextureInfo();
			textureInfos[i] = mTextureInfo[i];

			new (&textures[i]) SPtr<TextureCore>();

			if (mTextures[i].isLoaded())
				textures[i] = mTextures[i]->getCore();
			else
				textures[i] = nullptr;
		}

		for (UINT32 i = 0; i < mNumSamplerStates; i++)
		{
			new (&samplers[i]) SPtr<SamplerStateCore>();

			if (mSamplerStates[i] != nullptr)
				samplers[i] = mSamplerStates[i]->getCore();
			else
				samplers[i] = nullptr;
		}

		return CoreSyncData(data, totalSize);
	}

	void GpuParams::getListenerResources(Vector<HResource>& resources)
	{
		for (UINT32 i = 0; i < mNumTextures; i++)
		{
			if (mTextures[i] != nullptr)
				resources.push_back(mTextures[i]);
		}
	}
}