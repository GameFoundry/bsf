#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Material manager handles material creation.
	 */
	class CM_EXPORT MaterialManager : public Module<MaterialManager>
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