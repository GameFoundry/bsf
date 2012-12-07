#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmMatrix3.h"
#include "CmMatrix4.h"
#include "CmMaterial.h"
#include "CmGpuProgramParams.h"
#include "CmShader.h"
#include "CmDebug.h"
#include "CmException.h"
#include "CmKeyValuePair.h"

namespace CamelotEngine
{
	class CM_EXPORT MaterialRTTI : public RTTIType<Material, Resource, MaterialRTTI>
	{
	private:
		enum MaterialParamType
		{
			MPT_FLOAT,
			MPT_INT,
			MPT_TEX
		};

		struct FloatParam
		{
			FloatParam() {}

			FloatParam(UINT32 bufferIdx, GpuConstantType type, UINT32 count)
				:mBufferIdx(bufferIdx), mType(type), mCount(count)
			{ }

			UINT32 mBufferIdx;
			UINT32 mCount;
			GpuConstantType mType;
		};

		class FloatParamKVPRTTI;
		typedef KeyValuePair<String, FloatParam, FloatParamKVPRTTI> FloatParamKVP;

		class FloatParamKVPRTTI : public RTTIType<FloatParamKVP, IReflectable, FloatParamKVPRTTI>
		{
		private:
			String& getKey(FloatParamKVP* obj) { return obj->mKey; }
			void setKey(FloatParamKVP* obj,  String& val) { obj->mKey = val; }

			FloatParam& getValue(FloatParamKVP* obj) { return obj->mValue; }
			void setValue(FloatParamKVP* obj,  FloatParam& val) { obj->mValue = val; }

		public:
			FloatParamKVPRTTI()
			{
				addPlainField("mKey", 0, &FloatParamKVPRTTI::getKey, &FloatParamKVPRTTI::setKey);
				addPlainField("mValue", 1, &FloatParamKVPRTTI::getValue, &FloatParamKVPRTTI::setValue);
			}

		public:
			virtual const String& getRTTIName()
			{
				static String name = "FloatParamKVP";
				return name;
			}

			virtual UINT32 getRTTIId()
			{
				return TID_FloatParamKVP;
			}

			virtual std::shared_ptr<IReflectable> newRTTIObject()
			{
				return std::shared_ptr<FloatParamKVP>(new FloatParamKVP());
			}
		};

		class TexParamKVPRTTI;
		typedef KeyValuePair<String, TextureHandle, TexParamKVPRTTI> TexParamKVP;

		class TexParamKVPRTTI : public RTTIType<TexParamKVP, IReflectable, TexParamKVPRTTI>
		{
		private:
			String& getKey(TexParamKVP* obj) { return obj->mKey; }
			void setKey(TexParamKVP* obj,  String& val) { obj->mKey = val; }

			TextureHandle& getValue(TexParamKVP* obj) { return obj->mValue; }
			void setValue(TexParamKVP* obj,  TextureHandle& val) { obj->mValue = val; }

		public:
			TexParamKVPRTTI()
			{
				addPlainField("mKey", 0, &TexParamKVPRTTI::getKey, &TexParamKVPRTTI::setKey);
				addReflectableField("mValue", 1, &TexParamKVPRTTI::getValue, &TexParamKVPRTTI::setValue);
			}

		public:
			virtual const String& getRTTIName()
			{
				static String name = "TexParamKVP";
				return name;
			}

			virtual UINT32 getRTTIId()
			{
				return TID_MaterialTexParamKVP;
			}

			virtual std::shared_ptr<IReflectable> newRTTIObject()
			{
				return std::shared_ptr<TexParamKVP>(new TexParamKVP());
			}
		};

		class SamplerStateParamKVPRTTI;
		typedef KeyValuePair<String, SamplerState, SamplerStateParamKVPRTTI> SamplerStateKVP;

		class SamplerStateParamKVPRTTI : public RTTIType<SamplerStateKVP, IReflectable, SamplerStateParamKVPRTTI>
		{
		private:
			String& getKey(SamplerStateKVP* obj) { return obj->mKey; }
			void setKey(SamplerStateKVP* obj,  String& val) { obj->mKey = val; }

			SamplerState& getValue(SamplerStateKVP* obj) { return obj->mValue; }
			void setValue(SamplerStateKVP* obj,  SamplerState& val) { obj->mValue = val; }

		public:
			SamplerStateParamKVPRTTI()
			{
				addPlainField("mKey", 0, &SamplerStateParamKVPRTTI::getKey, &SamplerStateParamKVPRTTI::setKey);
				addPlainField("mValue", 1, &SamplerStateParamKVPRTTI::getValue, &SamplerStateParamKVPRTTI::setValue);
			}

		public:
			virtual const String& getRTTIName()
			{
				static String name = "SamplerStateKVP";
				return name;
			}

			virtual UINT32 getRTTIId()
			{
				return TID_SamplerStateParamKVP;
			}

			virtual std::shared_ptr<IReflectable> newRTTIObject()
			{
				return std::shared_ptr<SamplerStateKVP>(new SamplerStateKVP());
			}
		};

		class MaterialParamsRTTI;

		struct MaterialParams : public IReflectable
		{
			map<String, FloatParam>::type mFloatParams;
			map<String, TextureHandle>::type mTextureParams;
			map<String, SamplerState>::type mSamplerStateParams;

			vector<float>::type mFloatBuffer;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/

		public:
			friend class MaterialRTTI::MaterialParamsRTTI;

			static RTTITypeBase* getRTTIStatic();
			RTTITypeBase* getRTTI() const;
		};

		class MaterialParamsRTTI : public RTTIType<MaterialParams, IReflectable, MaterialParamsRTTI>
		{
		private:
			struct TempParams
			{
				vector<std::shared_ptr<FloatParamKVP>>::type mFloatParams;
				vector<std::shared_ptr<TexParamKVP>>::type mTexParams;
				vector<std::shared_ptr<SamplerStateKVP>>::type mSamplerStateParams;
			};

			std::shared_ptr<FloatParamKVP> getFloatParam(MaterialParams* obj, UINT32 idx)
			{
				UINT32 curIdx = 0;
				for(auto iter = obj->mFloatParams.begin(); iter != obj->mFloatParams.end(); ++iter)
				{
					if(curIdx == idx)
					{
						return std::shared_ptr<FloatParamKVP>(new FloatParamKVP(iter->first, iter->second));
					}

					curIdx++;
				}

				CM_EXCEPT(InternalErrorException, "Invalid index.");
			}

			void setFloatParam(MaterialParams* obj, UINT32 idx, std::shared_ptr<FloatParamKVP> value)
			{
				TempParams* tempParams = boost::any_cast<TempParams*>(obj->mRTTIData);
				tempParams->mFloatParams.push_back(value);
			}

			void setNumFloatParams(MaterialParams* obj, UINT32 size)
			{
				// Do nothing. Map is expanded automatically as entries are added
			}

			UINT32 getNumFloatParams(MaterialParams* obj)
			{
				return (UINT32)obj->mFloatParams.size();
			}

			std::shared_ptr<TexParamKVP> getTexParam(MaterialParams* obj, UINT32 idx)
			{
				UINT32 curIdx = 0;
				for(auto iter = obj->mTextureParams.begin(); iter != obj->mTextureParams.end(); ++iter)
				{
					if(curIdx == idx)
					{
						return std::shared_ptr<TexParamKVP>(new TexParamKVP(iter->first, iter->second));
					}

					curIdx++;
				}

				CM_EXCEPT(InternalErrorException, "Invalid index.");
			}

			void setTexParam(MaterialParams* obj, UINT32 idx, std::shared_ptr<TexParamKVP> value)
			{
				TempParams* tempParams = boost::any_cast<TempParams*>(obj->mRTTIData);
				tempParams->mTexParams.push_back(value);
			}

			void setNumTexParams(MaterialParams* obj, UINT32 size)
			{
				// Do nothing. Map is expanded automatically as entries are added
			}

			UINT32 getNumTexParams(MaterialParams* obj)
			{
				return (UINT32)obj->mTextureParams.size();
			}

			std::shared_ptr<SamplerStateKVP> getSamplerStateParam(MaterialParams* obj, UINT32 idx)
			{
				UINT32 curIdx = 0;
				for(auto iter = obj->mSamplerStateParams.begin(); iter != obj->mSamplerStateParams.end(); ++iter)
				{
					if(curIdx == idx)
					{
						return std::shared_ptr<SamplerStateKVP>(new SamplerStateKVP(iter->first, iter->second));
					}

					curIdx++;
				}

				CM_EXCEPT(InternalErrorException, "Invalid index.");
			}

			void setSamplerStateParam(MaterialParams* obj, UINT32 idx, std::shared_ptr<SamplerStateKVP> value)
			{
				TempParams* tempParams = boost::any_cast<TempParams*>(obj->mRTTIData);
				tempParams->mSamplerStateParams.push_back(value);
			}

			void setNumSamplerStateParams(MaterialParams* obj, UINT32 size)
			{
				// Do nothing. Map is expanded automatically as entries are added
			}

			UINT32 getNumSamplerStateParams(MaterialParams* obj)
			{
				return (UINT32)obj->mSamplerStateParams.size();
			}

			ManagedDataBlock getFloatBuffer(MaterialParams* obj)
			{
				UINT32 bufferSize = (UINT32)obj->mFloatBuffer.size();
				float* buffer = new float[bufferSize];

				for(UINT32 i = 0; i < bufferSize; i++)
					buffer[i] = obj->mFloatBuffer[i];

				return ManagedDataBlock((UINT8*)buffer, bufferSize, true);
			}

			void setFloatBuffer(MaterialParams* obj, ManagedDataBlock data)
			{
				obj->mFloatBuffer.clear();

				float* buffer = (float*)data.getData();
				for(UINT32 i = 0; i < data.getSize(); i++)
					obj->mFloatBuffer.push_back(buffer[i]);

				data.destroy();
			}

		public:
			MaterialParamsRTTI()
			{
				addReflectablePtrArrayField("mFloatParams", 0, &MaterialParamsRTTI::getFloatParam, &MaterialParamsRTTI::getNumFloatParams, 
					&MaterialParamsRTTI::setFloatParam, &MaterialParamsRTTI::setNumFloatParams);
				addReflectablePtrArrayField("mTexParams", 1, &MaterialParamsRTTI::getTexParam, &MaterialParamsRTTI::getNumTexParams, 
					&MaterialParamsRTTI::setTexParam, &MaterialParamsRTTI::setNumTexParams);
				addReflectablePtrArrayField("mSamplerStateParams", 2, &MaterialParamsRTTI::getSamplerStateParam, &MaterialParamsRTTI::getNumSamplerStateParams, 
					&MaterialParamsRTTI::setSamplerStateParam, &MaterialParamsRTTI::setNumSamplerStateParams);
				addDataBlockField("mFloatBuffer", 3, &MaterialParamsRTTI::getFloatBuffer, &MaterialParamsRTTI::setFloatBuffer);
			}

			virtual void onDeserializationStarted(IReflectable* obj)
			{
				MaterialParams* materialParams = static_cast<MaterialParams*>(obj);
				materialParams->mRTTIData = new TempParams();
			}

			virtual void onDeserializationEnded(IReflectable* obj)
			{
				// TODO Low priority:
				// Due to the way serialization is done I cannot guarantee "value"s key/value fields
				// have been set, so I need to delay accessing them until the end of serialization.
				// I'd really like to avoid this as its not intuitive to think in this way.

				MaterialParams* materialParams = static_cast<MaterialParams*>(obj);

				if(materialParams->mRTTIData.empty())
					return;

				TempParams* tempParams = boost::any_cast<TempParams*>(materialParams->mRTTIData);

				for(auto iter = tempParams->mFloatParams.begin(); iter != tempParams->mFloatParams.end(); ++iter)
				{
					std::shared_ptr<FloatParamKVP> floatParam = *iter;
					materialParams->mFloatParams[floatParam->mKey] = floatParam->mValue;
				}

				for(auto iter = tempParams->mTexParams.begin(); iter != tempParams->mTexParams.end(); ++iter)
				{
					std::shared_ptr<TexParamKVP> texParam = *iter;
					materialParams->mTextureParams[texParam->mKey] = texParam->mValue;
				}

				for(auto iter = tempParams->mSamplerStateParams.begin(); iter != tempParams->mSamplerStateParams.end(); ++iter)
				{
					std::shared_ptr<SamplerStateKVP> samplerStateParam = *iter;
					materialParams->mSamplerStateParams[samplerStateParam->mKey] = samplerStateParam->mValue;
				}

				materialParams->mRTTIData = nullptr;
			}

			virtual const String& getRTTIName()
			{
				static String name = "MaterialParams";
				return name;
			}

			virtual UINT32 getRTTIId()
			{
				return TID_MaterialParams;
			}

			virtual std::shared_ptr<IReflectable> newRTTIObject()
			{
				return std::shared_ptr<MaterialParams>(new MaterialParams());
			}
		};

		ShaderPtr getShader(Material* obj)
		{
			return obj->mShader;
		}

		void setShader(Material* obj,  ShaderPtr val)
		{
			obj->mShader = val;
		}

		std::shared_ptr<MaterialParams> getMaterialParams(Material* obj)
		{
			if(obj->mRTTIData.empty())
				return nullptr;

			return boost::any_cast<std::shared_ptr<MaterialParams>>(obj->mRTTIData);
		}

		void setMaterialParams(Material* obj, std::shared_ptr<MaterialParams> value)
		{
			obj->mRTTIData = value;
		}

	public:
		MaterialRTTI()
		{
			addReflectablePtrField("mShader", 0, &MaterialRTTI::getShader, &MaterialRTTI::setShader);
			addReflectablePtrField("mMaterialParams", 1, &MaterialRTTI::getMaterialParams, &MaterialRTTI::setMaterialParams);
		}

		virtual void onSerializationStarted(IReflectable* obj);
		virtual void onSerializationEnded(IReflectable* obj);
		virtual void onDeserializationStarted(IReflectable* obj);
		virtual void onDeserializationEnded(IReflectable* obj);

		virtual const String& getRTTIName()
		{
			static String name = "Material";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Material;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<Material>(new Material());
		}
	};
}