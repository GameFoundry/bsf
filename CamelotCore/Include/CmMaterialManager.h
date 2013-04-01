#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	class CM_EXPORT MaterialManager : public Module<MaterialManager>
	{
	public:
		MaterialPtr create() const;
		MaterialPtr create(ShaderPtr shader) const;
	};
}