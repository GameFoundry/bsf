#include "BsShader.h"
#include "BsTechnique.h"
#include "BsException.h"
#include "BsDebug.h"
#include "BsShaderRTTI.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	ShaderBase::ShaderBase(const String& name)
		:mName(name), mQueueSortType(QueueSortType::FrontToBack), mQueuePriority((UINT32)QueuePriority::Opaque),
		mSeparablePasses(true)
	{

	}

	bool ShaderBase::isTechniqueSupported(const SPtr<Technique> technique) const
	{
		return technique->isSupported();
	}

	bool ShaderBase::isTechniqueSupported(const SPtr<TechniqueCore> technique) const
	{
		return technique->isSupported();
	}

	void ShaderBase::checkBounds(UINT32 idx, UINT32 bound) const
	{
		if (idx < 0 || idx >= bound)
			BS_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx));
	}

	void ShaderBase::setQueueSortType(QueueSortType sortType) 
	{ 
		mQueueSortType = sortType;

		_markCoreDirty();
	}

	void ShaderBase::setQueuePriority(UINT32 priority) 
	{ 
		mQueuePriority = priority;

		_markCoreDirty();
	}

	void ShaderBase::setAllowSeparablePasses(bool enable) 
	{ 
		mSeparablePasses = enable;

		_markCoreDirty();
	}

	void ShaderBase::addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 rendererSemantic, UINT32 arraySize, UINT32 elementSize)
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

		_markCoreDirty();
	}

	void ShaderBase::addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, UINT32 rendererSemantic)
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

		_markCoreDirty();
	}

	GpuParamType ShaderBase::getParamType(const String& name) const
	{
		auto findIterData = mDataParams.find(name);
		if(findIterData != mDataParams.end())
			return GPT_DATA;

		auto findIterObject = mObjectParams.find(name);
		if(findIterObject != mObjectParams.end())
			return GPT_OBJECT;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	const SHADER_DATA_PARAM_DESC& ShaderBase::getDataParamDesc(const String& name) const
	{
		auto findIterData = mDataParams.find(name);
		if(findIterData != mDataParams.end())
			return findIterData->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	const SHADER_OBJECT_PARAM_DESC& ShaderBase::getObjectParamDesc(const String& name) const
	{
		auto findIterObject = mObjectParams.find(name);
		if(findIterObject != mObjectParams.end())
			return findIterObject->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	bool ShaderBase::hasDataParam(const String& name) const
	{
		auto findIterData = mDataParams.find(name);
		if(findIterData != mDataParams.end())
			return true;

		return false;
	}

	bool ShaderBase::hasObjectParam(const String& name) const
	{
		auto findIterObject = mObjectParams.find(name);
		if(findIterObject != mObjectParams.end())
			return true;

		return false;
	}

	void ShaderBase::removeParameter(const String& name)
	{
		mDataParams.erase(name);
		mObjectParams.erase(name);

		_markCoreDirty();
	}

	void ShaderBase::setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage, UINT32 rendererSemantic)
	{
		SHADER_PARAM_BLOCK_DESC desc;
		desc.name = name;
		desc.shared = shared;
		desc.usage = usage;
		desc.rendererSemantic = rendererSemantic;

		mParamBlocks[name] = desc;

		_markCoreDirty();
	}

	ShaderCore::ShaderCore(const String& name)
		:TShader(name)
	{

	}

	SPtr<TechniqueCore> ShaderCore::addTechnique(const String& renderSystem, const String& renderer)
	{
		SPtr<TechniqueCore> technique = TechniqueCore::create(renderSystem, renderer);

		mTechniques.push_back(technique);
		_markCoreDirty();

		return technique;
	}

	void ShaderCore::syncToCore(const CoreSyncData& data)
	{
		char* buffer = (char*)data.getBuffer();

		mDataParams.clear();
		mObjectParams.clear();
		mParamBlocks.clear();

		buffer = rttiReadElem(mQueueSortType, buffer);
		buffer = rttiReadElem(mQueuePriority, buffer);
		buffer = rttiReadElem(mSeparablePasses, buffer);
		buffer = rttiReadElem(mDataParams, buffer);
		buffer = rttiReadElem(mObjectParams, buffer);
		buffer = rttiReadElem(mParamBlocks, buffer);
	}

	SPtr<ShaderCore> ShaderCore::create(const String& name)
	{
		ShaderCore* shaderCore = new (bs_alloc<ShaderCore>()) ShaderCore(name);
		SPtr<ShaderCore> shaderCorePtr = bs_shared_ptr<ShaderCore, GenAlloc>(shaderCore);
		shaderCorePtr->_setThisPtr(shaderCorePtr);
		shaderCorePtr->initialize();

		return shaderCorePtr;
	}

	Shader::Shader(const String& name)
		:TShader(name)
	{

	}

	SPtr<Technique> Shader::addTechnique(const String& renderSystem, const String& renderer)
	{
		SPtr<Technique> technique = Technique::create(renderSystem, renderer);

		mTechniques.push_back(technique);
		_markCoreDirty();

		return technique;
	}

	void Shader::_markCoreDirty()
	{
		markCoreDirty();
	}

	CoreSyncData Shader::syncToCore(FrameAlloc* allocator)
	{
		UINT32 size = 0;
		size += rttiGetElemSize(mQueueSortType);
		size += rttiGetElemSize(mQueuePriority);
		size += rttiGetElemSize(mSeparablePasses);
		size += rttiGetElemSize(mDataParams);
		size += rttiGetElemSize(mObjectParams);
		size += rttiGetElemSize(mParamBlocks);

		UINT8* buffer = allocator->alloc(size);

		char* dataPtr = (char*)buffer;
		dataPtr = rttiWriteElem(mQueueSortType, dataPtr);
		dataPtr = rttiWriteElem(mQueuePriority, dataPtr);
		dataPtr = rttiWriteElem(mSeparablePasses, dataPtr);
		dataPtr = rttiWriteElem(mDataParams, dataPtr);
		dataPtr = rttiWriteElem(mObjectParams, dataPtr);
		dataPtr = rttiWriteElem(mParamBlocks, dataPtr);

		return CoreSyncData((UINT8*)buffer, size);
	}

	SPtr<ShaderCore> Shader::getCore() const
	{
		return std::static_pointer_cast<ShaderCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> Shader::createCore() const
	{
		ShaderCore* shaderCore = new (bs_alloc<ShaderCore>()) ShaderCore(mName);
		SPtr<ShaderCore> shaderCorePtr = bs_shared_ptr<ShaderCore, GenAlloc>(shaderCore);
		shaderCorePtr->_setThisPtr(shaderCorePtr);

		return shaderCorePtr;
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

	RTTITypeBase* Shader::getRTTIStatic()
	{
		return ShaderRTTI::instance();
	}

	RTTITypeBase* Shader::getRTTI() const
	{
		return Shader::getRTTIStatic();
	}
}