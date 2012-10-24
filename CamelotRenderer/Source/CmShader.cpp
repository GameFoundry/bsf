#include "CmShader.h"
#include "CmTechnique.h"
#include "CmException.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	Shader::Shader(const String& name)
		:mName(name)
	{

	}

	void Shader::addTechnique(TechniquePtr technique)
	{
		auto iterFind = std::find(mTechniques.begin(), mTechniques.end(), technique);

		if(iterFind != mTechniques.end())
			CM_EXCEPT(InvalidParametersException, "Identical technique already exists in this shader.");

		mTechniques.push_back(technique);
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
}