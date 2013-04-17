#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotFramework
{
	class CM_EXPORT MaterialManager : public Module<MaterialManager>
	{
	public:
		MaterialPtr create() const;
		MaterialPtr create(ShaderPtr shader) const;
	};
}