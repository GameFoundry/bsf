#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmMaterial.h"

namespace CamelotEngine
{
	class CM_EXPORT MaterialRTTI : public RTTIType<Material, Resource, MaterialRTTI>
	{
	private:
		ShaderPtr getShader(Material* obj)
		{
			return obj->getShader();
		}

		void setShader(Material* obj,  ShaderPtr val)
		{
			obj->setShader(val);
		}

	public:
		MaterialRTTI()
		{
			addReflectablePtrField("mShader", 0, &MaterialRTTI::getShader, &MaterialRTTI::setShader);
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