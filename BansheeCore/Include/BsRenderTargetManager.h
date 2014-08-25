#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles render target updates, usually cross-thread updates triggered
	 *			by render targets getting modified on the core thread.
	 *
	 * @note	Internal class.
	 */
	class BS_CORE_EXPORT RenderTargetManager : public Module<RenderTargetManager>
	{
		struct RenderTargetData
		{
			RenderTargetProperties* properties;
			bool dirty;
		};

	public:
		~RenderTargetManager();

		/**
		 * @brief	Registers a new render target with the manager.
		 *			Should be called when RT is initialized on the core thread.
		 *
		 * @note	Core thread only.
		 */
		void registerRenderTarget(RenderTarget* rt);

		/**
		 * @brief	Unregisters a render target from the manager.
		 *			Should be called when RT is getting destroyed on the core thread.
		 *
		 * @note	Core thread only.
		 */
		void unregisterRenderTarget(RenderTarget* rt);

		/**
		 * @brief	Called once per frame on the core thread. Should be called at the end of
		 *			the frame once all render target related methods have completed.
		 *
		 * @note	Core thread only.
		 */
		void updateCore();

		/**
		 * @brief	Called once per frame on the sim thread. Should be called at the beginning
		 *			of the frame before the code can access render targets.
		 */
		void update();

	private:
		UnorderedMap<RenderTarget*, RenderTargetData> mRenderTargets;
		BS_MUTEX(mMutex);
	};
}