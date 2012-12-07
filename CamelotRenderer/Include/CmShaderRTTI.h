#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmShader.h"

namespace CamelotEngine
{
	class CM_EXPORT ShaderRTTI : public RTTIType<Shader, Resource, ShaderRTTI>
	{
	private:
		TechniquePtr getTechnique(Shader* obj, UINT32 idx)
		{
			return obj->mTechniques[idx];
		}

		void setTechnique(Shader* obj, UINT32 idx, TechniquePtr val)
		{
			obj->mTechniques[idx] = val;
		}

		UINT32 getTechniqueArraySize(Shader* obj)
		{
			return (UINT32)obj->mTechniques.size();
		}

		void setTechniqueArraySize(Shader* obj, UINT32 size)
		{
			obj->mTechniques.resize(size);
		}

	public:
		ShaderRTTI()
		{
			addReflectablePtrArrayField("mTechniques", 0, &ShaderRTTI::getTechnique, &ShaderRTTI::getTechniqueArraySize, 
				&ShaderRTTI::setTechnique, &ShaderRTTI::setTechniqueArraySize);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Shader";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Shader;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return std::shared_ptr<Shader>(new Shader());
		}
	};
}