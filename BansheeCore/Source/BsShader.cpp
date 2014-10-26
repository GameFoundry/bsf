#include "BsShader.h"
#include "BsTechnique.h"
#include "BsException.h"
#include "BsDebug.h"
#include "BsShaderProxy.h"
#include "BsShaderRTTI.h"

namespace BansheeEngine
{
	Shader::Shader(const String& name)
		:mName(name), mQueueSortType(QueueSortType::FrontToBack), mQueuePriority((UINT32)QueuePriority::Opaque), 
		mSeparablePasses(true), mCoreDirtyFlags(0xFFFFFFFF)
	{

	}

	TechniquePtr Shader::addTechnique(const String& renderSystem, const String& renderer)
	{
		TechniquePtr technique = bs_shared_ptr<Technique, PoolAlloc>(renderSystem, renderer);
		mTechniques.push_back(technique);

		return technique;
	}

	void Shader::removeTechnique(UINT32 idx)
	{
		if(idx < 0 || idx >= mTechniques.size())
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));

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
			BS_EXCEPT(InvalidParametersException, "Cannot remove specified technique because it wasn't found in this shader.");

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

		return nullptr;

		// TODO - Low priority. Instead of returning null use an extremely simple technique that will be supported almost everywhere as a fallback.
	}

	void Shader::setQueueSortType(QueueSortType sortType) 
	{ 
		mQueueSortType = sortType;

		markCoreDirty();
	}

	void Shader::setQueuePriority(UINT32 priority) 
	{ 
		mQueuePriority = priority;

		markCoreDirty();
	}

	void Shader::setAllowSeparablePasses(bool enable) 
	{ 
		mSeparablePasses = enable;

		markCoreDirty();
	}

	void Shader::addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 rendererSemantic, UINT32 arraySize, UINT32 elementSize)
	{
		if(type == GPDT_STRUCT && elementSize <= 0)
			BS_EXCEPT(InvalidParametersException, "You need to provide a non-zero element size for a struct parameter.")

		SHADER_DATA_PARAM_DESC desc;
		desc.name = name;
		desc.gpuVariableName = gpuVariableName;
		desc.type = type;
		desc.arraySize = arraySize;
		desc.rendererSemantic = rendererSemantic;
		desc.elementSize = elementSize;

		mDataParams[name] = desc;
		mObjectParams.erase(name);

		markCoreDirty();
	}

	void Shader::addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, UINT32 rendererSemantic)
	{
		auto iterFind = mObjectParams.find(name);

		if (iterFind == mObjectParams.end())
		{
			SHADER_OBJECT_PARAM_DESC desc;
			desc.name = name;
			desc.type = type;
			desc.rendererSemantic = rendererSemantic;
			desc.gpuVariableNames.push_back(gpuVariableName);

			mObjectParams[name] = desc;
		}
		else
		{
			SHADER_OBJECT_PARAM_DESC& desc = iterFind->second;

			if (desc.type != type || desc.rendererSemantic != rendererSemantic)
				BS_EXCEPT(InvalidParametersException, "Shader parameter with the name \"" + name + "\" already exists with different properties.");

			Vector<String>& gpuVariableNames = desc.gpuVariableNames;
			bool found = false;
			for (UINT32 i = 0; i < (UINT32)gpuVariableNames.size(); i++)
			{
				if (gpuVariableNames[i] == gpuVariableName)
				{
					found = true;
					break;
				}
			}

			if (!found)
				gpuVariableNames.push_back(gpuVariableName);
		}

		mDataParams.erase(name);

		markCoreDirty();
	}

	GpuParamType Shader::getParamType(const String& name) const
	{
		auto findIterData = mDataParams.find(name);
		if(findIterData != mDataParams.end())
			return GPT_DATA;

		auto findIterObject = mObjectParams.find(name);
		if(findIterObject != mObjectParams.end())
			return GPT_OBJECT;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	const SHADER_DATA_PARAM_DESC& Shader::getDataParamDesc(const String& name) const
	{
		auto findIterData = mDataParams.find(name);
		if(findIterData != mDataParams.end())
			return findIterData->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	const SHADER_OBJECT_PARAM_DESC& Shader::getObjectParamDesc(const String& name) const
	{
		auto findIterObject = mObjectParams.find(name);
		if(findIterObject != mObjectParams.end())
			return findIterObject->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
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

	void Shader::setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage, UINT32 rendererSemantic)
	{
		SHADER_PARAM_BLOCK_DESC desc;
		desc.name = name;
		desc.shared = shared;
		desc.usage = usage;
		desc.rendererSemantic = rendererSemantic;

		mParamBlocks[name] = desc;

		markCoreDirty();
	}

	bool Shader::isSampler(GpuParamObjectType type)
	{
		switch(type)
		{
			case GPOT_SAMPLER1D:
			case GPOT_SAMPLER2D:
			case GPOT_SAMPLER3D:
			case GPOT_SAMPLERCUBE:
			case GPOT_SAMPLER2DMS:
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
		case GPOT_TEXTURE2DMS:
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
		ShaderPtr newShader = bs_core_ptr<Shader, PoolAlloc>(new (bs_alloc<Shader, PoolAlloc>()) Shader(name));
		newShader->_setThisPtr(newShader);
		newShader->initialize();

		return newShader;
	}

	ShaderProxyPtr Shader::_createProxy()
	{
		ShaderProxyPtr proxy = bs_shared_ptr<ShaderProxy>();
		proxy->dataParams = mDataParams;
		proxy->objectParams = mObjectParams;
		proxy->paramBlocks = mParamBlocks;
		proxy->queuePriority = mQueuePriority;
		proxy->queueSortType = mQueueSortType;
		proxy->separablePasses = mSeparablePasses;

		return proxy;
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