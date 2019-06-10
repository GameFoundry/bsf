//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsRenderECSPrerequisites.h"
#include "Renderer/BsRendererFactory.h"

namespace bs
{
	/** @addtogroup RenderECS
	 *  @{
	 */
	/**
	 * Renderer factory implementation that creates and initializes the default framework renderer. Used by the
	 * RendererManager.
	 */
	class RenderECSFactory : public RendererFactory
	{
	public:
		static constexpr const char* SystemName = "bsfECSRenderer";

		/** @copydoc RendererFactory::create */
		SPtr<ct::Renderer> create() override;

		/** @copydoc RendererFactory::name */
		const String& name() const override;
	};

	/** @} */
}
