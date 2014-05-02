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
#include "CmGpuParams.h"
#include "CmShader.h"
#include "CmDebug.h"
#include "CmException.h"

namespace BansheeEngine
{
	/************************************************************************/
	/* 					HELPER STRUCTS TO HELP SERIALIZING                  */
	/************************************************************************/

	class CM_EXPORT MaterialFloatParam : public IReflectable
	{
	public:
		String name;
		float value;
		UINT32 arrayIdx;

		friend class MaterialFloatParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialVec2Param : public IReflectable
	{
	public:
		String name;
		Vector2 value;
		UINT32 arrayIdx;

		friend class MaterialVec2ParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialVec3Param : public IReflectable
	{
	public:
		String name;
		Vector3 value;
		UINT32 arrayIdx;

		friend class MaterialVec3ParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialVec4Param : public IReflectable
	{
	public:
		String name;
		Vector4 value;
		UINT32 arrayIdx;

		friend class MaterialVec4ParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialMat3Param : public IReflectable
	{
	public:
		String name;
		Matrix3 value;
		UINT32 arrayIdx;

		friend class MaterialMat3ParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialMat4Param : public IReflectable
	{
	public:
		String name;
		Matrix4 value;
		UINT32 arrayIdx;

		friend class MaterialMat4ParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialStructParam : public IReflectable
	{
	public:
		String name;
		Material::StructData value;
		UINT32 arrayIdx;
		UINT32 elementSize;

		friend class MaterialStructParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialTextureParam : public IReflectable
	{
	public:
		String name;
		HTexture value;

		friend class MaterialTextureParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialSamplerStateParam : public IReflectable
	{
	public:
		String name;
		HSamplerState value;

		friend class MaterialSamplerStateParamRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	class CM_EXPORT MaterialParams : public IReflectable
	{
	public:
		Vector<MaterialFloatParam>::type floatParams;
		Vector<MaterialVec2Param>::type vec2Params;
		Vector<MaterialVec3Param>::type vec3Params;
		Vector<MaterialVec4Param>::type vec4Params;
		Vector<MaterialMat3Param>::type mat3Params;
		Vector<MaterialMat4Param>::type mat4Params;
		Vector<MaterialStructParam>::type structParams;
		Vector<MaterialTextureParam>::type textureParams;
		Vector<MaterialSamplerStateParam>::type samplerStateParams;

		friend class MaterialParamsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};

	/************************************************************************/
	/* 				RTTI FOR HELPER STRUCTS TO HELP SERIALIZING             */
	/************************************************************************/

	class CM_EXPORT MaterialFloatParamRTTI : public RTTIType<MaterialFloatParam, IReflectable, MaterialFloatParamRTTI>
	{
	public:
		String& getName(MaterialFloatParam* obj) { return obj->name; }
		void setName(MaterialFloatParam* obj, String& name) { obj->name = name; }

		float& getValue(MaterialFloatParam* obj) { return obj->value; }
		void setValue(MaterialFloatParam* obj, float& value) { obj->value = value; }

		UINT32& getArrayIdx(MaterialFloatParam* obj) { return obj->arrayIdx; }
		void setArrayIdx(MaterialFloatParam* obj, UINT32& value) { obj->arrayIdx = value; }

		MaterialFloatParamRTTI()
		{
			addPlainField("name", 0, &MaterialFloatParamRTTI::getName, &MaterialFloatParamRTTI::setName);
			addPlainField("value", 1, &MaterialFloatParamRTTI::getValue, &MaterialFloatParamRTTI::setValue);
			addPlainField("arrayIdx", 2, &MaterialFloatParamRTTI::getArrayIdx, &MaterialFloatParamRTTI::setArrayIdx);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialFloatParam";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamFloat; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialFloatParam, ScratchAlloc>(); 
		}
	};

	class CM_EXPORT MaterialVec2ParamRTTI : public RTTIType<MaterialVec2Param, IReflectable, MaterialVec2ParamRTTI>
	{
	public:
		String& getName(MaterialVec2Param* obj) { return obj->name; }
		void setName(MaterialVec2Param* obj, String& name) { obj->name = name; }

		Vector2& getValue(MaterialVec2Param* obj) { return obj->value; }
		void setValue(MaterialVec2Param* obj, Vector2& value) { obj->value = value; }

		UINT32& getArrayIdx(MaterialVec2Param* obj) { return obj->arrayIdx; }
		void setArrayIdx(MaterialVec2Param* obj, UINT32& value) { obj->arrayIdx = value; }

		MaterialVec2ParamRTTI()
		{
			addPlainField("name", 0, &MaterialVec2ParamRTTI::getName, &MaterialVec2ParamRTTI::setName);
			addPlainField("value", 1, &MaterialVec2ParamRTTI::getValue, &MaterialVec2ParamRTTI::setValue);
			addPlainField("arrayIdx", 2, &MaterialVec2ParamRTTI::getArrayIdx, &MaterialVec2ParamRTTI::setArrayIdx);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialVec2Param";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamVec2; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialVec2Param, ScratchAlloc>(); 
		}
	};

	class CM_EXPORT MaterialVec3ParamRTTI : public RTTIType<MaterialVec3Param, IReflectable, MaterialVec3ParamRTTI>
	{
	public:
		String& getName(MaterialVec3Param* obj) { return obj->name; }
		void setName(MaterialVec3Param* obj, String& name) { obj->name = name; }

		Vector3& getValue(MaterialVec3Param* obj) { return obj->value; }
		void setValue(MaterialVec3Param* obj, Vector3& value) { obj->value = value; }

		UINT32& getArrayIdx(MaterialVec3Param* obj) { return obj->arrayIdx; }
		void setArrayIdx(MaterialVec3Param* obj, UINT32& value) { obj->arrayIdx = value; }

		MaterialVec3ParamRTTI()
		{
			addPlainField("name", 0, &MaterialVec3ParamRTTI::getName, &MaterialVec3ParamRTTI::setName);
			addPlainField("value", 1, &MaterialVec3ParamRTTI::getValue, &MaterialVec3ParamRTTI::setValue);
			addPlainField("arrayIdx", 2, &MaterialVec3ParamRTTI::getArrayIdx, &MaterialVec3ParamRTTI::setArrayIdx);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialVec3Param";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamVec3; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialVec3Param, ScratchAlloc>(); 
		}
	};

	class CM_EXPORT MaterialVec4ParamRTTI : public RTTIType<MaterialVec4Param, IReflectable, MaterialVec4ParamRTTI>
	{
	public:
		String& getName(MaterialVec4Param* obj) { return obj->name; }
		void setName(MaterialVec4Param* obj, String& name) { obj->name = name; }

		Vector4& getValue(MaterialVec4Param* obj) { return obj->value; }
		void setValue(MaterialVec4Param* obj, Vector4& value) { obj->value = value; }

		UINT32& getArrayIdx(MaterialVec4Param* obj) { return obj->arrayIdx; }
		void setArrayIdx(MaterialVec4Param* obj, UINT32& value) { obj->arrayIdx = value; }

		MaterialVec4ParamRTTI()
		{
			addPlainField("name", 0, &MaterialVec4ParamRTTI::getName, &MaterialVec4ParamRTTI::setName);
			addPlainField("value", 1, &MaterialVec4ParamRTTI::getValue, &MaterialVec4ParamRTTI::setValue);
			addPlainField("arrayIdx", 2, &MaterialVec4ParamRTTI::getArrayIdx, &MaterialVec4ParamRTTI::setArrayIdx);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialVec4Param";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamVec4; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return cm_shared_ptr<MaterialVec4Param, ScratchAlloc>();
		}
	};

	class CM_EXPORT MaterialMat3ParamRTTI : public RTTIType<MaterialMat3Param, IReflectable, MaterialMat3ParamRTTI>
	{
	public:
		String& getName(MaterialMat3Param* obj) { return obj->name; }
		void setName(MaterialMat3Param* obj, String& name) { obj->name = name; }

		Matrix3& getValue(MaterialMat3Param* obj) { return obj->value; }
		void setValue(MaterialMat3Param* obj, Matrix3& value) { obj->value = value; }

		UINT32& getArrayIdx(MaterialMat3Param* obj) { return obj->arrayIdx; }
		void setArrayIdx(MaterialMat3Param* obj, UINT32& value) { obj->arrayIdx = value; }

		MaterialMat3ParamRTTI()
		{
			addPlainField("name", 0, &MaterialMat3ParamRTTI::getName, &MaterialMat3ParamRTTI::setName);
			addPlainField("value", 1, &MaterialMat3ParamRTTI::getValue, &MaterialMat3ParamRTTI::setValue);
			addPlainField("arrayIdx", 2, &MaterialMat3ParamRTTI::getArrayIdx, &MaterialMat3ParamRTTI::setArrayIdx);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialMat3Param";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamMat3; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialMat3Param, ScratchAlloc>();
		}
	};

	class CM_EXPORT MaterialMat4ParamRTTI : public RTTIType<MaterialMat4Param, IReflectable, MaterialMat4ParamRTTI>
	{
	public:
		String& getName(MaterialMat4Param* obj) { return obj->name; }
		void setName(MaterialMat4Param* obj, String& name) { obj->name = name; }

		Matrix4& getValue(MaterialMat4Param* obj) { return obj->value; }
		void setValue(MaterialMat4Param* obj, Matrix4& value) { obj->value = value; }

		UINT32& getArrayIdx(MaterialMat4Param* obj) { return obj->arrayIdx; }
		void setArrayIdx(MaterialMat4Param* obj, UINT32& value) { obj->arrayIdx = value; }

		MaterialMat4ParamRTTI()
		{
			addPlainField("name", 0, &MaterialMat4ParamRTTI::getName, &MaterialMat4ParamRTTI::setName);
			addPlainField("value", 1, &MaterialMat4ParamRTTI::getValue, &MaterialMat4ParamRTTI::setValue);
			addPlainField("arrayIdx", 2, &MaterialMat4ParamRTTI::getArrayIdx, &MaterialMat4ParamRTTI::setArrayIdx);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialMat4Param";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamMat4; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialMat4Param, ScratchAlloc>();
		}
	};

	class CM_EXPORT MaterialStructParamRTTI : public RTTIType<MaterialStructParam, IReflectable, MaterialStructParamRTTI>
	{
	public:
		String& getName(MaterialStructParam* obj) { return obj->name; }
		void setName(MaterialStructParam* obj, String& name) { obj->name = name; }

		ManagedDataBlock getValue(MaterialStructParam* obj) 
		{ 
			ManagedDataBlock returnValue(obj->value.size);

			UINT8* data = returnValue.getData();
			memcpy(data, obj->value.data.get(), obj->value.size);

			return returnValue; 
		}

		void setValue(MaterialStructParam* obj, ManagedDataBlock value) 
		{ 
			obj->value = Material::StructData(value.getSize()); 
			obj->value.write(value.getData());
			
		}

		UINT32& getArrayIdx(MaterialStructParam* obj) { return obj->arrayIdx; }
		void setArrayIdx(MaterialStructParam* obj, UINT32& value) { obj->arrayIdx = value; }

		UINT32& getElementSize(MaterialStructParam* obj) { return obj->elementSize; }
		void setElementSize(MaterialStructParam* obj, UINT32& value) { obj->elementSize = value; }

		MaterialStructParamRTTI()
		{
			addPlainField("name", 0, &MaterialStructParamRTTI::getName, &MaterialStructParamRTTI::setName);
			addDataBlockField("value", 1, &MaterialStructParamRTTI::getValue, &MaterialStructParamRTTI::setValue);
			addPlainField("arrayIdx", 2, &MaterialStructParamRTTI::getArrayIdx, &MaterialStructParamRTTI::setArrayIdx);
			addPlainField("elementSize", 3, &MaterialStructParamRTTI::getElementSize, &MaterialStructParamRTTI::setElementSize);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialStructParam";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamStruct; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialStructParam, ScratchAlloc>();
		}
	};

	class CM_EXPORT MaterialTextureParamRTTI : public RTTIType<MaterialTextureParam, IReflectable, MaterialTextureParamRTTI>
	{
	public:
		String& getName(MaterialTextureParam* obj) { return obj->name; }
		void setName(MaterialTextureParam* obj, String& name) { obj->name = name; }

		HTexture& getValue(MaterialTextureParam* obj) { return obj->value; }
		void setValue(MaterialTextureParam* obj, HTexture& value) { obj->value = value; }

		MaterialTextureParamRTTI()
		{
			addPlainField("name", 0, &MaterialTextureParamRTTI::getName, &MaterialTextureParamRTTI::setName);
			addReflectableField("value", 1, &MaterialTextureParamRTTI::getValue, &MaterialTextureParamRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialTextureParam";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamTexture; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialTextureParam, ScratchAlloc>();
		}
	};

	class CM_EXPORT MaterialSamplerStateParamRTTI : public RTTIType<MaterialSamplerStateParam, IReflectable, MaterialSamplerStateParamRTTI>
	{
	public:
		String& getName(MaterialSamplerStateParam* obj) { return obj->name; }
		void setName(MaterialSamplerStateParam* obj, String& name) { obj->name = name; }

		HSamplerState& getValue(MaterialSamplerStateParam* obj) { return obj->value; }
		void setValue(MaterialSamplerStateParam* obj, HSamplerState& value) { obj->value = value; }

		MaterialSamplerStateParamRTTI()
		{
			addPlainField("name", 0, &MaterialSamplerStateParamRTTI::getName, &MaterialSamplerStateParamRTTI::setName);
			addReflectableField("value", 1, &MaterialSamplerStateParamRTTI::getValue, &MaterialSamplerStateParamRTTI::setValue);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialSamplerStateParam";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParamSamplerState; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialSamplerStateParam, ScratchAlloc>();
		}
	};

	class CM_EXPORT MaterialParamsRTTI : public RTTIType<MaterialParams, IReflectable, MaterialParamsRTTI>
	{
	public:
		MaterialFloatParam& getFloatParam(MaterialParams* obj, UINT32 idx) { return obj->floatParams[idx]; }
		void setFloatParam(MaterialParams* obj, UINT32 idx, MaterialFloatParam& param) { obj->floatParams[idx] = param; }
		UINT32 getFloatArraySize(MaterialParams* obj) { return (UINT32)obj->floatParams.size(); }
		void setFloatArraySize(MaterialParams* obj, UINT32 size) { obj->floatParams.resize(size); }

		MaterialVec2Param& getVec2Param(MaterialParams* obj, UINT32 idx) { return obj->vec2Params[idx]; }
		void setVec2Param(MaterialParams* obj, UINT32 idx, MaterialVec2Param& param) { obj->vec2Params[idx] = param; }
		UINT32 getVec2ArraySize(MaterialParams* obj) { return (UINT32)obj->vec2Params.size(); }
		void setVec2ArraySize(MaterialParams* obj, UINT32 size) { obj->vec2Params.resize(size); }

		MaterialVec3Param& getVec3Param(MaterialParams* obj, UINT32 idx) { return obj->vec3Params[idx]; }
		void setVec3Param(MaterialParams* obj, UINT32 idx, MaterialVec3Param& param) { obj->vec3Params[idx] = param; }
		UINT32 getVec3ArraySize(MaterialParams* obj) { return (UINT32)obj->vec3Params.size(); }
		void setVec3ArraySize(MaterialParams* obj, UINT32 size) { obj->vec3Params.resize(size); }

		MaterialVec4Param& getVec4Param(MaterialParams* obj, UINT32 idx) { return obj->vec4Params[idx]; }
		void setVec4Param(MaterialParams* obj, UINT32 idx, MaterialVec4Param& param) { obj->vec4Params[idx] = param; }
		UINT32 getVec4ArraySize(MaterialParams* obj) { return (UINT32)obj->vec4Params.size(); }
		void setVec4ArraySize(MaterialParams* obj, UINT32 size) { obj->vec4Params.resize(size); }

		MaterialMat3Param& getMat3Param(MaterialParams* obj, UINT32 idx) { return obj->mat3Params[idx]; }
		void setMat3Param(MaterialParams* obj, UINT32 idx, MaterialMat3Param& param) { obj->mat3Params[idx] = param; }
		UINT32 getMat3ArraySize(MaterialParams* obj) { return (UINT32)obj->mat3Params.size(); }
		void setMat3ArraySize(MaterialParams* obj, UINT32 size) { obj->mat3Params.resize(size); }

		MaterialMat4Param& getMat4Param(MaterialParams* obj, UINT32 idx) { return obj->mat4Params[idx]; }
		void setMat4Param(MaterialParams* obj, UINT32 idx, MaterialMat4Param& param) { obj->mat4Params[idx] = param; }
		UINT32 getMat4ArraySize(MaterialParams* obj) { return (UINT32)obj->mat4Params.size(); }
		void setMat4ArraySize(MaterialParams* obj, UINT32 size) { obj->mat4Params.resize(size); }

		MaterialStructParam& getStructParam(MaterialParams* obj, UINT32 idx) { return obj->structParams[idx]; }
		void setStructParam(MaterialParams* obj, UINT32 idx, MaterialStructParam& param) { obj->structParams[idx] = param; }
		UINT32 getStructArraySize(MaterialParams* obj) { return (UINT32)obj->structParams.size(); }
		void setStructArraySize(MaterialParams* obj, UINT32 size) { obj->structParams.resize(size); }

		MaterialTextureParam& getTextureParam(MaterialParams* obj, UINT32 idx) { return obj->textureParams[idx]; }
		void setTextureParam(MaterialParams* obj, UINT32 idx, MaterialTextureParam& param) { obj->textureParams[idx] = param; }
		UINT32 getTextureArraySize(MaterialParams* obj) { return (UINT32)obj->textureParams.size(); }
		void setTextureArraySize(MaterialParams* obj, UINT32 size) { obj->textureParams.resize(size); }

		MaterialSamplerStateParam& getSamplerStateParam(MaterialParams* obj, UINT32 idx) { return obj->samplerStateParams[idx]; }
		void setSamplerStateParam(MaterialParams* obj, UINT32 idx, MaterialSamplerStateParam& param) { obj->samplerStateParams[idx] = param; }
		UINT32 getSamplerStateArraySize(MaterialParams* obj) { return (UINT32)obj->samplerStateParams.size(); }
		void setSamplerStateArraySize(MaterialParams* obj, UINT32 size) { obj->samplerStateParams.resize(size); }

		MaterialParamsRTTI()
		{
			addReflectableArrayField("floatParams", 0, &MaterialParamsRTTI::getFloatParam, 
				&MaterialParamsRTTI::getFloatArraySize, &MaterialParamsRTTI::setFloatParam, &MaterialParamsRTTI::setFloatArraySize);

			addReflectableArrayField("vec2Params", 1, &MaterialParamsRTTI::getVec2Param, 
				&MaterialParamsRTTI::getVec2ArraySize, &MaterialParamsRTTI::setVec2Param, &MaterialParamsRTTI::setVec2ArraySize);

			addReflectableArrayField("vec3Params", 2, &MaterialParamsRTTI::getVec3Param, 
				&MaterialParamsRTTI::getVec3ArraySize, &MaterialParamsRTTI::setVec3Param, &MaterialParamsRTTI::setVec3ArraySize);

			addReflectableArrayField("vec4Params", 3, &MaterialParamsRTTI::getVec4Param, 
				&MaterialParamsRTTI::getVec4ArraySize, &MaterialParamsRTTI::setVec4Param, &MaterialParamsRTTI::setVec4ArraySize);

			addReflectableArrayField("mat3Params", 4, &MaterialParamsRTTI::getMat3Param, 
				&MaterialParamsRTTI::getMat3ArraySize, &MaterialParamsRTTI::setMat3Param, &MaterialParamsRTTI::setMat3ArraySize);

			addReflectableArrayField("mat4Params", 5, &MaterialParamsRTTI::getMat4Param, 
				&MaterialParamsRTTI::getMat4ArraySize, &MaterialParamsRTTI::setMat4Param, &MaterialParamsRTTI::setMat4ArraySize);

			addReflectableArrayField("structParams", 6, &MaterialParamsRTTI::getStructParam, 
				&MaterialParamsRTTI::getStructArraySize, &MaterialParamsRTTI::setStructParam, &MaterialParamsRTTI::setStructArraySize);

			addReflectableArrayField("textureParams", 7, &MaterialParamsRTTI::getTextureParam, 
				&MaterialParamsRTTI::getTextureArraySize, &MaterialParamsRTTI::setTextureParam, &MaterialParamsRTTI::setTextureArraySize);

			addReflectableArrayField("samplerStateParams", 8, &MaterialParamsRTTI::getSamplerStateParam, 
				&MaterialParamsRTTI::getSamplerStateArraySize, &MaterialParamsRTTI::setSamplerStateParam, &MaterialParamsRTTI::setSamplerStateArraySize);
		}

		virtual const String& getRTTIName()
		{
			static String name = "MaterialParams";
			return name;
		}

		virtual UINT32 getRTTIId() { return TID_MaterialParams; }
		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{ 
			return cm_shared_ptr<MaterialParams, ScratchAlloc>();
		}
	};

	class CM_EXPORT MaterialRTTI : public RTTIType<Material, Resource, MaterialRTTI>
	{
	private:
		ShaderPtr getShader(Material* obj)
		{
			return obj->mShader;
		}

		void setShader(Material* obj, ShaderPtr val)
		{
			obj->mShader = val;
		}

		std::shared_ptr<MaterialParams> getMaterialParams(Material* obj)
		{
			if(obj->mRTTIData.empty())
				return nullptr;

			return any_cast<std::shared_ptr<MaterialParams>>(obj->mRTTIData);
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

		virtual std::shared_ptr<IReflectable> newRTTIObject();
	};
}