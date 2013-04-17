#pragma once

#include "CmPrerequisites.h"
#include "CmRendererFactory.h"
#include "CmModule.h"

namespace CamelotFramework
{
	class CM_EXPORT RendererManager : public Module<RendererManager>
	{
	public:
		void setActive(const String& name);
		RendererPtr getActive() { return mActiveRenderer; }

		/**
		 * @brief	Core renderer represents a set of shared features within all renderers.
		 * 			Techniques using this renderer name will report as if they are supported regardless
		 * 			of the active renderer.
		 *
		 * @return	The core renderer name.
		 */
		static const String& getCoreRendererName();

		void registerFactory(RendererFactoryPtr factory);
	private:
		std::vector<RendererFactoryPtr> mAvailableFactories;

		RendererPtr mActiveRenderer;
	};
}

