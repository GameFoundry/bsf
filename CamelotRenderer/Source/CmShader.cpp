#include "CmShader.h"
#include "CmTechnique.h"
#include "CmException.h"
#include "CmDebug.h"
#include "CmShaderRTTI.h"

namespace CamelotEngine
{
	Shader::Shader(const String& name)
		:mName(name)
	{

	}

	void Shader::initialize_internal()
	{	}

	TechniquePtr Shader::addTechnique(const String& renderSystem, const String& renderer)
	{
		TechniquePtr technique = TechniquePtr(new Technique(renderSystem, renderer));
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

	void Shader::addParameter(const String& name, const String& gpuVariableName, GpuParamDataType type, UINT32 arraySize, bool hidden)
	{
		SHADER_DATA_PARAM_DESC desc;
		desc.name = name;
		desc.gpuVariableName = gpuVariableName;
		desc.type = type;
		desc.arraySize = arraySize;
		desc.hidden = hidden;

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

	void Shader::removeParameter(const String& name)
	{
		mDataParams.erase(name);
		mObjectParams.erase(name);
	}

	void Shader::setParamBlockAttribs(const String& name, bool shared, GpuParamBlockUsage usage)
	{
		SHADER_PARAM_BLOCK_DESC desc;
		desc.shared = shared;
		desc.usage = usage;

		mParamBlocks[name] = desc;
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