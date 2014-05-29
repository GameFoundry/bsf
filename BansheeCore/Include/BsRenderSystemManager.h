#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsRenderSystemFactory.h"

namespace BansheeEngine
{
	typedef std::shared_ptr<RenderSystemFactory> RenderSystemFactoryPtr;

	/**
	 * @brief	Manager that handles render system start up.
	 */
	class BS_CORE_EXPORT RenderSystemManager : public Module<RenderSystemManager>
	{
	public:
		RenderSystemManager();
		~RenderSystemManager();

		/**
		 * @brief	Starts the render system with the provided name and creates the primary render window.
		 *
		 * @param	name				Name of the render system to start. Factory for this render system must be previously
		 *								registered.
		 * @param	primaryWindowDesc	Contains options used for creating the primary window.
		 *
		 * @returns	Created render window if initialization is successful, null otherwise.
		 */
		RenderWindowPtr initialize(const String& name, RENDER_WINDOW_DESC& primaryWindowDesc);

		/**
		 * @brief	Registers a new render system factory responsible for creating a specific render system type.
		 */
		void registerRenderSystemFactory(RenderSystemFactoryPtr factory);
	private:
		Vector<RenderSystemFactoryPtr> mAvailableFactories;
		bool mRenderSystemInitialized;
	};
}

