//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Utility/BsModule.h"

namespace bs
{
	/** @addtogroup Material-Internal
	 *  @{
	 */

	/**	Material manager handles material creation. */
	class BS_CORE_EXPORT MaterialManager : public Module<MaterialManager>
	{
	public:
		/**
		 * Creates a new material without any assigned shader.
		 *
		 * @note	Make sure to call Material::setShader() before using it.
		 */
		SPtr<Material> create() const;

		/** Creates a new material with the specified shader. */
		SPtr<Material> create(const HShader& shader) const;

		/**
		 * Creates a new empty material without initializing it.
		 *
		 * @note	You must manually call Material::initialize() after creation.
		 */
		SPtr<Material> createEmpty() const;
	};

	/** @} */
}