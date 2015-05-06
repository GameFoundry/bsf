#include "BsScriptShader.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoArray.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	// Note: This must match C# ShaderParameterType enum
	enum class ShaderParameterType
	{
		Float, Vector2, Vector3, Vector4, Color,
		Matrix3, Matrix4, Texture2D,
		Texture3D, TextureCube, Sampler
	};

	ScriptShader::ScriptShader(MonoObject* instance, const HShader& shader)
		:ScriptObject(instance), mShader(shader)
	{

	}

	void ScriptShader::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetShaderParameters", &ScriptShader::internal_GetShaderParameters);
	}

	void ScriptShader::internal_GetShaderParameters(ScriptShader* nativeInstance, MonoArray** outNames, MonoArray** outTypes)
	{
		HShader shader = nativeInstance->getShaderHandle();
		if (!shader.isLoaded())
			return;

		const Map<String, SHADER_DATA_PARAM_DESC>& dataParams = shader->getDataParams();
		const Map<String, SHADER_OBJECT_PARAM_DESC>& textureParams = shader->getTextureParams();
		const Map<String, SHADER_OBJECT_PARAM_DESC>& samplerParams = shader->getSamplerParams();

		struct ParamInfo
		{
			String name;
			ShaderParameterType type;
		};

		Vector<ParamInfo> paramInfos;

		// TODO - Ignoring int, bool, struct and non-square matrices
		// TODO - Ignoring buffers and load/store textures
		for (auto& param : dataParams)
		{
			// TODO - No way to identify color types

			ShaderParameterType type;
			bool isValidType = false;
			switch (param.second.type) 
			{
			case GPDT_FLOAT1:
				type = ShaderParameterType::Float;
				isValidType = true;
				break;
			case GPDT_FLOAT2:
				type = ShaderParameterType::Vector2;
				isValidType = true;
				break;
			case GPDT_FLOAT3:
				type = ShaderParameterType::Vector3;
				isValidType = true;
				break;
			case GPDT_FLOAT4:
				type = ShaderParameterType::Vector4;
				isValidType = true;
				break;
			case GPDT_MATRIX_3X3:
				type = ShaderParameterType::Matrix3;
				isValidType = true;
				break;
			case GPDT_MATRIX_4X4:
				type = ShaderParameterType::Matrix4;
				isValidType = true;
				break;
			}

			if (isValidType)
				paramInfos.push_back({ param.first, type });
		}

		for (auto& param : textureParams)
		{
			ShaderParameterType type;
			bool isValidType = false;
			switch (param.second.type)
			{
			case GPOT_TEXTURE2D:
			case GPOT_TEXTURE2DMS:
				type = ShaderParameterType::Texture2D;
				isValidType = true;
				break;
			case GPOT_TEXTURE3D:
				type = ShaderParameterType::Texture3D;
				isValidType = true;
				break;
			case GPOT_TEXTURECUBE:
				type = ShaderParameterType::TextureCube;
				isValidType = true;
				break;
			}

			if (isValidType)
				paramInfos.push_back({ param.first, type });
		}

		for (auto& param : samplerParams)
		{
			ShaderParameterType type = ShaderParameterType::Sampler;
			paramInfos.push_back({ param.first, type });
		}


		UINT32 totalNumParams = (UINT32)paramInfos.size();

		ScriptArray names = ScriptArray::create<String>(totalNumParams);
		ScriptArray types = ScriptArray::create<UINT32>(totalNumParams);

		UINT32 idx = 0;
		for (auto& param : paramInfos)
		{
			names.set(idx, param.name);
			types.set(idx, param.type);

			idx++;
		}

		*outNames = names.getInternal();
		*outTypes = types.getInternal();
	}
	
	void ScriptShader::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptShader::setNativeHandle(const HResource& resource)
	{
		mShader = static_resource_cast<Shader>(resource);
	}
}