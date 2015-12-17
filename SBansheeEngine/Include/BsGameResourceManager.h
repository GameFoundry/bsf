#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interface that can be implemented by the resource loaders required
	 *			by GameResources.
	 */
	class BS_SCR_BE_EXPORT IGameResourceLoader
	{
	public:
		virtual ~IGameResourceLoader() { }

		/**
		 * @brief	Loads the resource at the specified path.
		 */
		virtual HResource load(const Path& path, bool keepLoaded) const = 0;
	};

	/**
	 * @brief	Handles loading of game resources when the standalone game is running.
	 */
	class BS_SCR_BE_EXPORT StandaloneResourceLoader : public IGameResourceLoader
	{
	public:
		/**
		 * @copydoc	IGameResourceLoader::load
		 */
		HResource load(const Path& path, bool keepLoaded) const override;
	};

	/**
	 * @brief	Keeps track of resources that can be dynamically loaded
	 *			during runtime. These resources will be packed with the game
	 *			build so that they're available on demand.
	 *
	 *			Internal resource handle can be overridden so that editor or other
	 *			systems can handle resource loading more directly.
	 */
	class BS_SCR_BE_EXPORT GameResourceManager : public Module<GameResourceManager>
	{
	public:
		GameResourceManager();

		/**
		 * @brief	Loads the resource at the specified path.
		 * 			
		 * @see	Resources::load
		 */
		HResource load(const Path& path, bool keepLoaded) const;	

		/**
		 * @copydoc	load
		 */
		template <class T>
		ResourceHandle<T> load(const Path& filePath, bool keepLoaded)
		{
			return static_resource_cast<T>(load(filePath, keepLoaded));
		}

		/**
		 * @brief	Sets the resource loader implementation that determines how are the
		 *			paths provided to ::load loaded.
		 */
		void setLoader(const SPtr<IGameResourceLoader>& loader);

	private:
		SPtr<IGameResourceLoader> mLoader;
	};
}