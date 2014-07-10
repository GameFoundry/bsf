//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Material manager handles material creation.
	 */
	class BS_CORE_EXPORT MaterialManager : public Module<MaterialManager>
	{
	public:
		/**
		 * @brief	Creates a new material without any assigned shader.
		 *
		 * @note	Make sure to call Material::setShader before using it.
		 */
		MaterialPtr create() const;

		/**
		 * @brief	Creates a new material with the specified shader.
		 */
		MaterialPtr create(ShaderPtr shader) const;
	};
}