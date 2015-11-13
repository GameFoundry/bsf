#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsRendererMaterial.h"

namespace BansheeEngine
{
	class RendererMaterialBase;
	struct RendererMaterialMetaData;

	/**
	 * @brief	Initializes and handles all renderer materials.
	 */
	class BS_EXPORT RendererMaterialManager : public Module<RendererMaterialManager>
	{
		/**
		 * @brief	Information used for initializing a renderer material managed by this module.
		 */	
		struct RendererMaterialData
		{
			RendererMaterialMetaData* metaData;
			Path shaderPath;
		};

	public:
		RendererMaterialManager();
		~RendererMaterialManager();

		/**
		 * @brief	Registers a new material that should be initialized on module start-up.
		 */
		static void _registerMaterial(RendererMaterialMetaData* metaData, const Path& shaderPath);

	private:
		template<class T>
		friend class RendererMaterial;

		/**
		 * @brief	Initializes all materials on the core thread.
		 */
		static void initOnCore(const Vector<SPtr<ShaderCore>>& shaders);

		/**
		 * @brief	Destroys all materials on the core thread.
		 */
		static void destroyOnCore();

		/**
		 * @brief	Returns a list in which are all materials managed by this module.
		 */
		static Vector<RendererMaterialData>& getMaterials();

		/**
		 * @brief	Returns a mutex used for inter-thread access to the materials list.
		 */
		static Mutex& getMutex();
	};
}