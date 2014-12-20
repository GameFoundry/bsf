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
		MaterialPtr create(const HShader& shader) const;

		/**
		 * @brief	Creates a new empty material without initializing it.
		 *
		 * @note	You must manually call initialize() after creation.
		 */
		MaterialPtr createEmpty() const;
	};
}