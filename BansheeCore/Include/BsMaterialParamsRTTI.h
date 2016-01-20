//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsMaterialParams.h"
#include "BsSamplerState.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT TextureParamDataRTTI : public RTTIType<MaterialParams::TextureParamData, IReflectable, TextureParamDataRTTI>
	{
	public:
		HTexture& getTexture(MaterialParams::TextureParamData* obj) { return obj->value; }
		void setTexture(MaterialParams::TextureParamData* obj, HTexture& value) { obj->value = value; }

		bool& getIsLoadStore(MaterialParams::TextureParamData* obj) { return obj->isLoadStore; }
		void setIsLoadStore(MaterialParams::TextureParamData* obj, bool& value) { obj->isLoadStore = value; }

		TextureSurface& getSurface(MaterialParams::TextureParamData* obj) { return obj->surface; }
		void setSurface(MaterialParams::TextureParamData* obj, TextureSurface& value) { obj->surface = value; }

		TextureParamDataRTTI()
		{
			addReflectableField("texture", 0, &TextureParamDataRTTI::getTexture, &TextureParamDataRTTI::setTexture);
			addPlainField("isLoadStore", 1, &TextureParamDataRTTI::getIsLoadStore, &TextureParamDataRTTI::setIsLoadStore);
			addPlainField("surface", 2, &TextureParamDataRTTI::getSurface, &TextureParamDataRTTI::setSurface);
		}

		const String& getRTTIName() override
		{
			static String name = "TextureParamData";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TextureParamData;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<MaterialParams::TextureParamData>();
		}
	};

	class BS_CORE_EXPORT StructParamDataRTTI : public RTTIType<MaterialParams::StructParamData, IReflectable, StructParamDataRTTI>
	{
	public:
		ManagedDataBlock getDataBuffer(MaterialParams::StructParamData* obj)
		{
			return ManagedDataBlock(obj->data, obj->dataSize);
		}

		void setDataBuffer(MaterialParams::StructParamData* obj, ManagedDataBlock value)
		{
			// Do nothing as the buffer was already assigned when it was allocated
		}

		static UINT8* allocateDataBuffer(MaterialParams::StructParamData* obj, UINT32 numBytes)
		{
			obj->data = (UINT8*)bs_alloc(numBytes);
			obj->dataSize = numBytes;

			return obj->data;
		}

		StructParamDataRTTI()
		{
			addDataBlockField("dataBuffer", 0, &StructParamDataRTTI::getDataBuffer,
				&StructParamDataRTTI::setDataBuffer, 0, &StructParamDataRTTI::allocateDataBuffer);
		}

		const String& getRTTIName() override
		{
			static String name = "StructParamData";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_StructParamData;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<MaterialParams::StructParamData>();
		}
	};

	class BS_CORE_EXPORT MaterialParamsRTTI : public RTTIType<MaterialParams, IReflectable, MaterialParamsRTTI>
	{
	public:
		struct MaterialParam
		{
			String name;
			MaterialParams::ParamData data;
		};

		MaterialParam& getParamData(MaterialParams* obj, UINT32 idx)
		{
			Vector<MaterialParam>& params = any_cast_ref<Vector<MaterialParam>>(obj->mRTTIData);
			return params[idx];
		}

		void setParamData(MaterialParams* obj, UINT32 idx, MaterialParam& param)
		{
			obj->mParams[param.name] = param.data;
		}

		UINT32 getParamDataArraySize(MaterialParams* obj)
		{
			Vector<MaterialParam>& params = any_cast_ref<Vector<MaterialParam>>(obj->mRTTIData);
			return (UINT32)params.size();
		}

		void setParamDataArraySize(MaterialParams* obj, UINT32 size)
		{
			// Do nothing
		}

		ManagedDataBlock getDataBuffer(MaterialParams* obj)
		{
			return ManagedDataBlock(obj->mDataParamsBuffer, obj->mDataSize);
		}

		void setDataBuffer(MaterialParams* obj, ManagedDataBlock value)
		{
			// Do nothing as the buffer was already assigned when it was allocated
		}

		static UINT8* allocateDataBuffer(MaterialParams* obj, UINT32 numBytes)
		{
			obj->mDataParamsBuffer = obj->mAlloc.alloc(numBytes);
			obj->mDataSize = numBytes;

			return obj->mDataParamsBuffer;
		}

		MaterialParams::StructParamData& getStructParam(MaterialParams* obj, UINT32 idx) { return obj->mStructParams[idx]; }
		void setStructParam(MaterialParams* obj, UINT32 idx, MaterialParams::StructParamData& param)
		{
			MaterialParams::StructParamData& newStructParam = obj->mStructParams[idx];
			newStructParam.data = (UINT8*)obj->mAlloc.alloc(param.dataSize);
			memcpy(newStructParam.data, param.data, param.dataSize);
			newStructParam.dataSize = param.dataSize;

			bs_free(param.data);
			param.data = nullptr;
		}
		UINT32 getStructArraySize(MaterialParams* obj) { return (UINT32)obj->mNumStructParams; }
		void setStructArraySize(MaterialParams* obj, UINT32 size)
		{
			obj->mNumStructParams = size;
			obj->mStructParams = obj->mAlloc.construct<MaterialParams::StructParamData>(size);
		}

		MaterialParams::TextureParamData& getTextureParam(MaterialParams* obj, UINT32 idx) { return obj->mTextureParams[idx]; }
		void setTextureParam(MaterialParams* obj, UINT32 idx, MaterialParams::TextureParamData& param) { obj->mTextureParams[idx] = param; }
		UINT32 getTextureArraySize(MaterialParams* obj) { return (UINT32)obj->mNumTextureParams; }
		void setTextureArraySize(MaterialParams* obj, UINT32 size)
		{
			obj->mNumTextureParams = size;
			obj->mTextureParams = obj->mAlloc.construct<MaterialParams::TextureParamData>(size);
		}

		SamplerStatePtr getSamplerStateParam(MaterialParams* obj, UINT32 idx) { return obj->mSamplerStateParams[idx]; }
		void setSamplerStateParam(MaterialParams* obj, UINT32 idx, SamplerStatePtr param) { obj->mSamplerStateParams[idx] = param; }
		UINT32 getSamplerStateArraySize(MaterialParams* obj) { return (UINT32)obj->mNumSamplerParams; }
		void setSamplerStateArraySize(MaterialParams* obj, UINT32 size)
		{
			obj->mNumSamplerParams = size;
			obj->mSamplerStateParams = obj->mAlloc.construct<SamplerStatePtr>(size);
		}

		MaterialParamsRTTI()
		{
			addPlainArrayField("paramData", 0, &MaterialParamsRTTI::getParamData, &MaterialParamsRTTI::getParamDataArraySize, 
				&MaterialParamsRTTI::setParamData, &MaterialParamsRTTI::setParamDataArraySize);

			addDataBlockField("dataBuffer", 1, &MaterialParamsRTTI::getDataBuffer,
				&MaterialParamsRTTI::setDataBuffer, 0, &MaterialParamsRTTI::allocateDataBuffer);

			addReflectableArrayField("structParams", 2, &MaterialParamsRTTI::getStructParam,
				&MaterialParamsRTTI::getStructArraySize, &MaterialParamsRTTI::setStructParam, &MaterialParamsRTTI::setStructArraySize);

			addReflectableArrayField("textureParams", 3, &MaterialParamsRTTI::getTextureParam,
				&MaterialParamsRTTI::getTextureArraySize, &MaterialParamsRTTI::setTextureParam, &MaterialParamsRTTI::setTextureArraySize);

			addReflectablePtrArrayField("samplerStateParams", 4, &MaterialParamsRTTI::getSamplerStateParam,
				&MaterialParamsRTTI::getSamplerStateArraySize, &MaterialParamsRTTI::setSamplerStateParam, &MaterialParamsRTTI::setSamplerStateArraySize);
		}

		void onSerializationStarted(IReflectable* obj) override
		{
			MaterialParams* paramsObj = static_cast<MaterialParams*>(obj);
			Vector<MaterialParam> params;

			for(auto& entry : paramsObj->mParams)
				params.push_back({ entry.first, entry.second });

			paramsObj->mRTTIData = params;
		}

		void onSerializationEnded(IReflectable* obj) override
		{
			MaterialParams* paramsObj = static_cast<MaterialParams*>(obj);
			paramsObj->mRTTIData = nullptr;
		}

		const String& getRTTIName() override
		{
			static String name = "MaterialParams";
			return name;
		}

		UINT32 getRTTIId() override 
		{ 
			return TID_MaterialParams; 
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<MaterialParams>();
		}
	};

	template<> struct RTTIPlainType<MaterialParamsRTTI::MaterialParam>
	{	
		enum { id = TID_MaterialRTTIParam }; enum { hasDynamicSize = 1 };

		static void toMemory(const MaterialParamsRTTI::MaterialParam& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			memory = rttiWriteElem(size, memory);
			memory = rttiWriteElem(data.name, memory);
			memory = rttiWriteElem(data.data, memory);
		}

		static UINT32 fromMemory(MaterialParamsRTTI::MaterialParam& data, char* memory)
		{ 
			UINT32 size = 0;
			
			memory = rttiReadElem(size, memory);
			memory = rttiReadElem(data.name, memory);
			memory = rttiReadElem(data.data, memory);

			return size;
		}

		static UINT32 getDynamicSize(const MaterialParamsRTTI::MaterialParam& data)
		{ 
			UINT64 dataSize = rttiGetElemSize(data.name) + rttiGetElemSize(data.data) + sizeof(UINT32);

#if BS_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				__string_throwDataOverflowException();
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	/** @} */
	/** @endcond */
}