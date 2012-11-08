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

		class FloatParamKVPRTTI : public KeyValuePairRTTI<String, FloatParam>
		{
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
		typedef KeyValuePair<String, TextureRef, TexParamKVPRTTI> TexParamKVP;

		class TexParamKVPRTTI : public KeyValuePairRTTI<String, TextureRef>
		{
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

		struct MaterialParams : public IReflectable
		{
			map<String, FloatParam>::type mFloatParams;
			map<String, TextureRef>::type mTextureParams;

			vector<float>::type mFloatBuffer;

			/************************************************************************/
			/* 								RTTI		                     		*/
			/************************************************************************/

		public:
			friend class MaterialParamsRTTI;

			static RTTITypeBase* getRTTIStatic();
			RTTITypeBase* getRTTI() const;
		};

		class MaterialParamsRTTI : public RTTIType<MaterialParams, IReflectable, MaterialParamsRTTI>
		{
		private:
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
				obj->mFloatParams[value->mKey] = value->mValue;
			}

			void setNumFloatParams(MaterialParams* obj, UINT32 size)
			{
				// Do nothing. Map is expanded automatically as entries are added
			}

			UINT32 getNumFloatParams(MaterialParams* obj)
			{
				obj->mFloatParams.size();
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
				obj->mTextureParams[value->mKey] = value->mValue;
			}

			void setNumTexParams(MaterialParams* obj, UINT32 size)
			{
				// Do nothing. Map is expanded automatically as entries are added
			}

			UINT32 getNumTexParams(MaterialParams* obj)
			{
				obj->mTextureParams.size();
			}

			ManagedDataBlock getFloatBuffer(MaterialParams* obj)
			{
				size_t bufferSize = obj->mFloatBuffer.size();
				float* buffer = new float[bufferSize];

				for(size_t i = 0; i < bufferSize; i++)
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
			return obj->getShader();
		}

		void setShader(Material* obj,  ShaderPtr val)
		{
			obj->setShader(val);
		}

		std::shared_ptr<MaterialParams> getMaterialParams(Material* obj)
		{
			return boost::any_cast<std::shared_ptr<MaterialParams>>(obj->mRTTIData);
		}

		void setMaterialParams(Material* obj, std::shared_ptr<MaterialParams> value)
		{
			obj->mRTTIData = value;
		}

		virtual void onSerializationStarted(IReflectable* obj);
		virtual void onSerializationEnded(IReflectable* obj);
		virtual void onDeserializationStarted(IReflectable* obj);
		virtual void onDeserializationEnded(IReflectable* obj);

	public:
		MaterialRTTI()
		{
			addReflectablePtrField("mShader", 0, &MaterialRTTI::getShader, &MaterialRTTI::setShader);
			addReflectablePtrField("mMaterialParams", 1, &MaterialRTTI::getMaterialParams, &MaterialRTTI::setMaterialParams);
		}

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