#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	/**
	 * @brief	Provides access to various materials that are required for core engine systems. 
	 * 			Each render system implementation needs to provide its own implementation of this
	 * 			module.
	 */
	class CM_EXPORT BuiltinMaterialManager : public Module<BuiltinMaterialManager>
	{
	public:
		virtual MaterialHandle createSpriteTextMaterial() const = 0;
		virtual MaterialHandle createSpriteImageMaterial() const = 0;
	};
}