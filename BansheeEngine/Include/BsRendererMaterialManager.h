#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsRendererMaterial.h"

namespace BansheeEngine
{
	class RendererMaterialBase;

	/**
	 * @brief	Initializes and handles all renderer materials.
	 */
	class BS_EXPORT RendererMaterialManager : public Module<RendererMaterialManager>
	{
	public:
		RendererMaterialManager();

	private:
		template<class T>
		friend class RendererMaterial;

		/**
		 * @brief	Registers a new material that should be initialized on module start-up.
		 */
		static void _registerMaterial(RendererMaterialBase* material);

		/**
		 * @brief	Initializes all materials on the core thread.
		 */
		static void initOnCore(const Vector<SPtr<ShaderCore>>& shaders);

		static Vector<RendererMaterialBase*> mMaterialsToInit;
		static Mutex mMutex;
	};
}