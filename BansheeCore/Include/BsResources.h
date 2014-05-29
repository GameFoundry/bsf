#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manager for dealing with all engine resources. It allows you to save 
	 *			new resources and load existing ones.
	 *
	 *			Used for manually dealing with resources but also for automatic resolving of
	 *			resource handles.
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT Resources : public Module<Resources>
	{
	public:
		Resources();
		~Resources();

		/**
		 * @brief	Loads the resource from a given path. Returns an empty handle if resource can't be loaded.
		 *			Resource is loaded synchronously.
		 */
		HResource load(const Path& filePath);

		/**
		 * @brief	Loads the resource asynchronously. Initially returned resource handle will be invalid
		 *			until resource loading is done.
		 *
		 * @param	filePath	Full pathname of the file.
		 * 						
		 * @note	You can use returned invalid handle in engine systems as the engine will check for handle 
		 *			validity before using it.
		 */
		HResource loadAsync(const Path& filePath);

		/**
		 * @brief	Loads the resource with the given UUID. Returns an empty handle if resource can't be loaded.
		 *			Resource is loaded synchronously.
		 */
		HResource loadFromUUID(const String& uuid);

		/**
		* @brief	Loads the resource with the given UUID asynchronously. Initially returned resource handle will be invalid
		*			until resource loading is done.
		 *
		 * @param	uuid	UUID of the resource to load. 
		 *
		 * @note	You can use returned invalid handle in engine systems as the engine will check for handle
		 *			validity before using it.
		 */
		HResource loadFromUUIDAsync(const String& uuid);

		/**
		 * @brief	Unloads the resource that is referenced by the handle. 
		 *
		 * @param	resourceHandle	Handle of the resource to unload.
		 * 							
		 * @note	GPU resources held by the resource will be scheduled to be destroyed on the core thread.
		 * 			Actual resource pointer wont be deleted until all user-held references to it are removed.
		 */
		void unload(HResource resource);

		/**
		 * @brief	Finds all resources that aren't being referenced anywhere and unloads them.
		 */
		void unloadAllUnused();

		/**
		 * @brief	Saves the resource at the specified location.
		 *
		 * @param	resource 	Handle to the resource.
		 * @param	filePath 	Full pathname of the file to save as.
		 * @param	overwrite	(optional) If true, any existing resource at the specified location will
		 * 						be overwritten.
		 * 						
		 * @note	If the resource is a GpuResource and you are in some way modifying it from the Core thread, make
		 * 			sure all those commands are submitted before you call this method. Otherwise an obsolete
		 * 			version of the resource might get saved.
		 *
		 *			If saving a core thread resource this is a potentially very slow operation as we must wait on the 
		 *			core thread and the GPU in order to read the resource.
		 */
		void save(HResource resource, const Path& filePath, bool overwrite);

		/**
		 * @brief	Creates a new resource handle from a resource pointer. 
		 *
		 * @note	Internal method used primarily be resource factory methods.
		 */
		HResource _createResourceHandle(const ResourcePtr& obj);

		/**
		 * @brief	Allows you to set a resource manifest containing UUID <-> file path mapping that is
		 * 			used when resolving resource references.
		 *
		 * @note	If you want objects that reference resources (using ResourceHandles) to be able to
		 * 			find that resource even after application restart, then you must save the resource
		 * 			manifest before closing the application and restore it upon startup.
		 * 			Otherwise resources will be assigned brand new UUIDs and references will be broken.
		 */
		void registerResourceManifest(const ResourceManifestPtr& manifest);

		/**
		 * @brief	Allows you to retrieve resource manifest containing UUID <-> file path mapping that is
		 * 			used when resolving resource references.
		 * 			
		 * @note	Resources module internally holds a "Default" manifest that it automatically updated whenever
		 * 			a resource is saved.
		 *
		 * @see		registerResourceManifest
		 */
		ResourceManifestPtr getResourceManifest(const String& name) const;

		/**
		 * @brief	Attempts to retrieve file path from the provided UUID. Returns true
		 *			if successful, false otherwise.
		 */
		bool getFilePathFromUUID(const String& uuid, Path& filePath) const;

		/**
		 * @brief	Attempts to retrieve UUID from the provided file path. Returns true
		 *			if successful, false otherwise.
		 */
		bool getUUIDFromFilePath(const Path& path, String& uuid) const;

	private:
		/**
		 * @brief	Starts resource loading or returns an already loaded resource.
		 */
		HResource loadInternal(const Path& filePath, bool synchronous);

		/**
		 * @brief	Performs actually reading and deserializing of the resource file. 
		 *			Called from various worker threads.
		 */
		ResourcePtr loadFromDiskAndDeserialize(const Path& filePath);

		/**
		 * @brief	Callback triggered when the task manager is ready to process the loading task.
		 */
		void loadCallback(const Path& filePath, HResource& resource);

	private:
		Vector<ResourceManifestPtr> mResourceManifests;
		ResourceManifestPtr mDefaultResourceManifest;

		BS_MUTEX(mInProgressResourcesMutex);
		BS_MUTEX(mLoadedResourceMutex);

		UnorderedMap<String, HResource> mLoadedResources;
		UnorderedMap<String, HResource> mInProgressResources; // Resources that are being asynchronously loaded
	};

	/**
	 * @brief	Provides easy access to the resource manager.
	 */
	BS_CORE_EXPORT Resources& gResources();
}