#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsRenderAPIFactory.h"

namespace BansheeEngine
{
	typedef std::shared_ptr<RenderAPIFactory> RenderAPIFactoryPtr;

	/**
	 * @brief	Manager that handles render system start up.
	 */
	class BS_CORE_EXPORT RenderAPIManager : public Module<RenderAPIManager>
	{
	public:
		RenderAPIManager();
		~RenderAPIManager();

		/**
		 * @brief	Starts the render API with the provided name and creates the primary render window.
		 *
		 * @param	name				Name of the render system to start. Factory for this render system must be previously
		 *								registered.
		 * @param	primaryWindowDesc	Contains options used for creating the primary window.
		 *
		 * @returns	Created render window if initialization is successful, null otherwise.
		 */
		RenderWindowPtr initialize(const String& name, RENDER_WINDOW_DESC& primaryWindowDesc);

		/**
		 * @brief	Registers a new render API factory responsible for creating a specific render system type.
		 */
		void registerFactory(RenderAPIFactoryPtr factory);
	private:
		Vector<RenderAPIFactoryPtr> mAvailableFactories;
		bool mRenderSystemInitialized;
	};
}

