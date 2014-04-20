#include "CmShader.h"
#include "CmTechnique.h"
#include "CmException.h"
#include "CmDebug.h"
#include "CmShaderRTTI.h"

namespace BansheeEngine
{
	Shader::Shader(const String& name)
		:mName(name)
	{

	}

	TechniquePtr Shader::addTechnique(const String& renderSystem, const String& renderer)
	{
		TechniquePtr technique = cm_shared_ptr<Technique, PoolAlloc>(renderSystem, renderer);
		mTechniques.push_back(technique);

		return technique;
	}

	void Shader::removeTechnique(UINT32 idx)
	{
		if(idx < 0 || idx >= mTechniques.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));

		int count = 0;
		auto iter = mTechniques.begin();
		while(count != idx)
		{
			++count;
			++iter;
		}

		mTechniques.erase(iter);
	}

	void Shader::removeTechnique(TechniquePtr technique)
	{
		auto iterFind = std::find(mTechniques.begin(), mTechniques.end(), technique);

		if(iterFind == mTechniques.end())
			CM_EXCEPT(InvalidParametersException, "Cannot remove specified technique because it wasn't found in this shader.");

		mTechniques.erase(iterFind);
	}

	TechniquePtr Shader::getBestTechnique() const
	{
		for(auto iter = mTechniques.begin(); iter != mTechniques.end(); ++iter)
		{
			if((*iter)->isSupported())
			{
				return *iter;
			}
		}

		CM_EXCEPT(InternalErrorException, "No techniques are supported!");

		// TODO - Low priority. Instead of throwing an exception use an extremely simple technique that will be supported almost everywhere as a fallback.
	}

	void Shader::addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 arraySize, UINT32 elementSize, bool hidden)
	{
		if(type == GPDT_STRUCT && elementSize <= 0)
			CM_EXCEPT(InvalidParametersException, "You need to provide a non-zero element size for a struct parameter.")

		SHADER_DATA_PARAM_DESC desc;
		desc.name = name;
		desc.gpuVariableName = gpuVariableName;
		desc.type = type;
		desc.arraySize = arraySize;
		desc.hidden = hidden;
		desc.elementSize = elementSize;

		mDataParams[name] = desc;
		mObjectParams.erase(name);
	}

	void Shader::addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, bool hidden)
	{
		SHADER_OBJECT_PARAM_DESC desc;
		desc.name = name;
		desc.gpuVariableName = gpuVariableName;
		desc.type = type;
		desc.hidden = hidden;

		mObjectParams[name] = desc;
		mDataParams.erase(name);
	}

	GpuParamType Shader::getParamType(const String& name) const
	{
		auto findIterData = mDataParams.find(name);
		if(findIterData != mDataParams.end())
			return GPT_DATA;

		auto findIterObject = mObjectParams.find(name);
		if(findIterObject != mObjectParams.end())
			return GPT_OBJECT;

		CM_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	const SHADER_DATA_PARAM_DESC& Shader::getDataParamDesc(const String& name) const
	{
		auto findIterData = mDataParams.find(name);
		if(findIterData != mDataParams.end())
			return findIterData->second;

		CM_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	const SHADER_OBJECT_PARAM_DESC& Shader::getObjectParamDesc(const String& name) const
	{
		auto findIterObject = mObjectParams.find(name);
		if(findIterObject != mObjectParams.end())
			return findIterObject->second;

		CM_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	bool Shader::hasDataParam(const String& name) const
	{
		auto findIterData = mDataParams.find(name);
		if(findIterData != mDataParams.end())
			return true;

		return false;
	}

	bool Shader::hasObjectParam(const String& name) const
	{
		auto findIterObject = mObjectParams.find(name);
		if(findIterObject != mObjectParams.end())
			return true;

		return false;
	}

	void Shader::removeParameter(const String& name)
	{
		mDataParams.erase(name);
		mObjectParams.erase(name);
	}

	void Shader::setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage)
	{
		SHADER_PARAM_BLOCK_DESC desc;
		desc.name = name;
		desc.shared = shared;
		desc.usage = usage;

		mParamBlocks[name] = desc;
	}

	bool Shader::isSampler(GpuParamObjectType type)
	{
		switch(type)
		{
			case GPOT_SAMPLER1D:
			case GPOT_SAMPLER2D:
			case GPOT_SAMPLER3D:
			case GPOT_SAMPLERCUBE:
				return true;
		}

		return false;
	}

	bool Shader::isTexture(GpuParamObjectType type)
	{
		switch(type)
		{
		case GPOT_TEXTURE1D:
		case GPOT_TEXTURE2D:
		case GPOT_TEXTURE3D:
		case GPOT_TEXTURECUBE:
		case GPOT_RWTEXTURE1D:
		case GPOT_RWTEXTURE2D:
		case GPOT_RWTEXTURE3D:
			return true;
		}

		return false;
	}

	bool Shader::isBuffer(GpuParamObjectType type)
	{
		switch(type)
		{
		case GPOT_BYTE_BUFFER:
		case GPOT_STRUCTURED_BUFFER:
		case GPOT_RWBYTE_BUFFER:
		case GPOT_RWAPPEND_BUFFER:
		case GPOT_RWCONSUME_BUFFER:
		case GPOT_RWSTRUCTURED_BUFFER:
		case GPOT_RWSTRUCTURED_BUFFER_WITH_COUNTER:
		case GPOT_RWTYPED_BUFFER:
			return true;
		}

		return false;
	}

	ShaderPtr Shader::create(const String& name)
	{
		ShaderPtr newShader = cm_core_ptr<Shader, PoolAlloc>(new (cm_alloc<Shader, PoolAlloc>()) Shader(name));
		newShader->setThisPtr(newShader);
		newShader->initialize();

		return newShader;
	}

	RTTITypeBase* Shader::getRTTIStatic()
	{
		return ShaderRTTI::instance();
	}

	RTTITypeBase* Shader::getRTTI() const
	{
		return Shader::getRTTIStatic();
	}
}