#include "BsShader.h"
#include "BsTechnique.h"
#include "BsException.h"
#include "BsDebug.h"
#include "BsShaderRTTI.h"
#include "BsResources.h"
#include "BsFrameAlloc.h"
#include "BsPass.h"

namespace BansheeEngine
{
	void SHADER_DESC::addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 rendererSemantic, UINT32 arraySize, UINT32 elementSize)
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

		dataParams[name] = desc;
		objectParams.erase(name);
	}

	void SHADER_DESC::addParameter(const String& name, const String& gpuVariableName, GpuParamObjectType type, UINT32 rendererSemantic)
	{
		auto iterFind = objectParams.find(name);

		if (iterFind == objectParams.end())
		{
			SHADER_OBJECT_PARAM_DESC desc;
			desc.name = name;
			desc.type = type;
			desc.rendererSemantic = rendererSemantic;
			desc.gpuVariableNames.push_back(gpuVariableName);

			objectParams[name] = desc;
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

		dataParams.erase(name);
	}

	void SHADER_DESC::setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage, UINT32 rendererSemantic)
	{
		SHADER_PARAM_BLOCK_DESC desc;
		desc.name = name;
		desc.shared = shared;
		desc.usage = usage;
		desc.rendererSemantic = rendererSemantic;

		paramBlocks[name] = desc;
	}

	ShaderBase::ShaderBase(const String& name, const SHADER_DESC& desc)
		:mName(name), mDesc(desc)
	{

	}

	GpuParamType ShaderBase::getParamType(const String& name) const
	{
		auto findIterData = mDesc.dataParams.find(name);
		if (findIterData != mDesc.dataParams.end())
			return GPT_DATA;

		auto findIterObject = mDesc.objectParams.find(name);
		if (findIterObject != mDesc.objectParams.end())
			return GPT_OBJECT;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	const SHADER_DATA_PARAM_DESC& ShaderBase::getDataParamDesc(const String& name) const
	{
		auto findIterData = mDesc.dataParams.find(name);
		if (findIterData != mDesc.dataParams.end())
			return findIterData->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	const SHADER_OBJECT_PARAM_DESC& ShaderBase::getObjectParamDesc(const String& name) const
	{
		auto findIterObject = mDesc.objectParams.find(name);
		if (findIterObject != mDesc.objectParams.end())
			return findIterObject->second;

		BS_EXCEPT(InternalErrorException, "Cannot find the parameter with the name: " + name);
	}

	bool ShaderBase::hasDataParam(const String& name) const
	{
		auto findIterData = mDesc.dataParams.find(name);
		if (findIterData != mDesc.dataParams.end())
			return true;

		return false;
	}

	bool ShaderBase::hasObjectParam(const String& name) const
	{
		auto findIterObject = mDesc.objectParams.find(name);
		if (findIterObject != mDesc.objectParams.end())
			return true;

		return false;
	}

	template<bool Core>
	TShader<Core>::TShader(const String& name, const SHADER_DESC& desc, const Vector<SPtr<TechniqueType>>& techniques)
		:ShaderBase(name, desc), mTechniques(techniques)
	{ }

	template<bool Core>
	TShader<Core>::~TShader() 
	{ }

	template<bool Core>
	SPtr<typename TShader<Core>::TechniqueType> TShader<Core>::getBestTechnique() const
	{
		for (auto iter = mTechniques.begin(); iter != mTechniques.end(); ++iter)
		{
			if ((*iter)->isSupported())
			{
				return *iter;
			}
		}

		return nullptr;

		// TODO - Low priority. Instead of returning null use an extremely simple technique that will be supported almost everywhere as a fallback.
	}

	template class TShader < false > ;
	template class TShader < true >;

	ShaderCore::ShaderCore(const String& name, const SHADER_DESC& desc, const Vector<SPtr<TechniqueCore>>& techniques)
		:TShader(name, desc, techniques)
	{

	}

	SPtr<ShaderCore> ShaderCore::create(const String& name, const SHADER_DESC& desc, const Vector<SPtr<TechniqueCore>>& techniques)
	{
		ShaderCore* shaderCore = new (bs_alloc<ShaderCore>()) ShaderCore(name, desc, techniques);
		SPtr<ShaderCore> shaderCorePtr = bs_shared_ptr<ShaderCore, GenAlloc>(shaderCore);
		shaderCorePtr->_setThisPtr(shaderCorePtr);
		shaderCorePtr->initialize();

		return shaderCorePtr;
	}

	Shader::Shader(const String& name, const SHADER_DESC& desc, const Vector<SPtr<Technique>>& techniques)
		:TShader(name, desc, techniques)
	{

	}

	SPtr<ShaderCore> Shader::getCore() const
	{
		return std::static_pointer_cast<ShaderCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> Shader::createCore() const
	{
		Vector<SPtr<TechniqueCore>> techniques;
		for (auto& technique : mTechniques)
			techniques.push_back(technique->getCore());

		ShaderCore* shaderCore = new (bs_alloc<ShaderCore>()) ShaderCore(mName, mDesc, techniques);
		SPtr<ShaderCore> shaderCorePtr = bs_shared_ptr<ShaderCore, GenAlloc>(shaderCore);
		shaderCorePtr->_setThisPtr(shaderCorePtr);

		return shaderCorePtr;
	}

	bool Shader::areDependenciesLoaded() const
	{
		TechniquePtr bestTechnique = getBestTechnique();
		if (bestTechnique == nullptr) // No valid technique, so everything is technically loaded
			return true;

		UINT32 numPasses = bestTechnique->getNumPasses();
		for (UINT32 i = 0; i < numPasses; i++)
		{
			PassPtr pass = bestTechnique->getPass(i);

			HGpuProgram vertProg = pass->getVertexProgram();
			if (vertProg != nullptr && !vertProg.isLoaded())
				return false;

			HGpuProgram fragProg = pass->getFragmentProgram();
			if (fragProg != nullptr && !fragProg.isLoaded())
				return false;

			HGpuProgram geomProg = pass->getGeometryProgram();
			if (geomProg != nullptr && !geomProg.isLoaded())
				return false;

			HGpuProgram domProg = pass->getDomainProgram();
			if (domProg != nullptr && !domProg.isLoaded())
				return false;

			HGpuProgram hullProg = pass->getHullProgram();
			if (hullProg != nullptr && !hullProg.isLoaded())
				return false;

			HGpuProgram computeProg = pass->getComputeProgram();
			if (computeProg != nullptr && !computeProg.isLoaded())
				return false;

			HBlendState blendState = pass->getBlendState();
			if (blendState != nullptr && !blendState.isLoaded())
				return false;

			HRasterizerState rasterizerState = pass->getRasterizerState();
			if (rasterizerState != nullptr && !rasterizerState.isLoaded())
				return false;

			HDepthStencilState depthStencilState = pass->getDepthStencilState();
			if (depthStencilState != nullptr && !depthStencilState.isLoaded())
				return false;
		}

		return true;
	}

	void Shader::getCoreDependencies(Vector<SPtr<CoreObject>>& dependencies)
	{
		for (auto& technique : mTechniques)
			dependencies.push_back(technique);
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

	HShader Shader::create(const String& name, const SHADER_DESC& desc, const Vector<SPtr<Technique>>& techniques)
	{
		ShaderPtr newShader = bs_core_ptr<Shader, PoolAlloc>(new (bs_alloc<Shader, PoolAlloc>()) Shader(name, desc, techniques));
		newShader->_setThisPtr(newShader);
		newShader->initialize();

		return static_resource_cast<Shader>(gResources()._createResourceHandle(newShader));
	}

	ShaderPtr Shader::createEmpty()
	{
		ShaderPtr newShader = bs_core_ptr<Shader, PoolAlloc>(new (bs_alloc<Shader, PoolAlloc>()) Shader());
		newShader->_setThisPtr(newShader);

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