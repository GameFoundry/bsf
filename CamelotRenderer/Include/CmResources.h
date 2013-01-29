#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"
#include "CmWorkQueue.h"

namespace CamelotEngine
{
	class CM_EXPORT Resources : public Module<Resources>
	{
	private:
		class CM_EXPORT ResourceRequestHandler : public WorkQueue::RequestHandler
		{
			virtual bool canHandleRequest( const WorkQueue::Request* req, const WorkQueue* srcQ );
			virtual WorkQueue::Response* handleRequest( const WorkQueue::Request* req, const WorkQueue* srcQ );
		};

		class CM_EXPORT ResourceResponseHandler : public WorkQueue::ResponseHandler
		{
			virtual bool canHandleResponse( const WorkQueue::Response* res, const WorkQueue* srcQ );
			virtual void handleResponse( const WorkQueue::Response* res, const WorkQueue* srcQ );
		};

		struct CM_EXPORT ResourceLoadRequest
		{
			String filePath;
			BaseResourceHandle resource;
		};

		struct CM_EXPORT ResourceLoadResponse
		{
			ResourcePtr rawResource;
		};

		struct CM_EXPORT ResourceAsyncOp
		{
			BaseResourceHandle resource;
			WorkQueue::RequestID requestID;
		};

		typedef std::shared_ptr<ResourceLoadRequest> ResourceLoadRequestPtr;
		typedef std::shared_ptr<ResourceLoadResponse> ResourceLoadResponsePtr;

	public:
		/**
		 * @brief	Constructor.
		 *
		 * @param	resMetaPath		Folder where the resource meta-data will be stored. If the folder doesn't exist
		 * 							it will be created.
		 */
		Resources(const String& metaDataFolder);
		~Resources();

		/**
		 * @brief	Called every frame by the main loop.
		 */
		void update();

		/**
		 * @brief	Loads the resource from a given path. Returns null if resource can't be loaded.
		 *
		 * @param	filePath					The path of the file to load. The file is searched for in
		 * 										the AssetDatabase first, and if it cannot be found it is
		 * 										loaded as a temporary resource object. You can't save
		 * 										references to temporary resource objects because they won't
		 * 										persist after application shut-down, but otherwise they act
		 * 										the same as normal resources.
		 *
		 * @return	Loaded resource, or null if it cannot be found.
		 */
		BaseResourceHandle load(const String& filePath);

		/**
		 * @brief	Loads the resource asynchronously. Initially returned resource should not be used
		 * 			until BaseResourceHandle.isResolved gets set to true. (Set only at the end of each frame)
		 *
		 * @param	filePath	Full pathname of the file.
		 * 						
		 * @return	Resource where the data will eventually be loaded, or null if the file cannot be found.
		 */
		BaseResourceHandle loadAsync(const String& filePath);

		/**
		 * @brief	Loads the resource with the given uuid.
		 *
		 * @param	uuid	UUID of the resource to load. 
		 *
		 * @return	Loaded resource, or null if it cannot be found.
		 */
		BaseResourceHandle loadFromUUID(const String& uuid);

		/**
		* @brief	Loads the resource with the given UUID asynchronously. Initially returned resource should not be used
		* 			until BaseResourceHandle.isResolved gets set to true. (Set only at the end of each frame)
		 *
		 * @param	uuid	UUID of the resource to load. 
		 *
		 * @return	Resource where the data will eventually be loaded, or null if the file cannot be found.
		 */
		BaseResourceHandle loadFromUUIDAsync(const String& uuid);

		/**
		 * @brief	Unloads the resource that is referenced by the handle.
		 *
		 * @param	resourceHandle	Handle of the resource.
		 * 							
		 * @note	GPU resources held by the resource will be scheduled to be destroyed on the render thread.
		 * 			Actual resource pointer wont be deleted until all user-held references to it are removed.
		 */
		void unload(BaseResourceHandle resource);

		/**
		 * @brief	Finds all resources that aren't being referenced anywhere and unloads them.
		 */
		void unloadAllUnused();

		/**
		 * @brief	Saves the resource. Resource must be registered using Resources::create beforehand.
		 *
		 * @param	resource   	The resource.
		 */
		void save(BaseResourceHandle resource);

		/**
		 * @brief	Creates a new resource at the specified location. Throws an exception if resource already exists.
		 * 			Automatically calls Resources::save.
		 *
		 * @param	resource 	The resource.
		 * @param	filePath 	Full pathname of the file.
		 * @param	overwrite	(optional) If true, any existing resource at the specified location
		 * 						will be overwritten.
		 */
		void create(BaseResourceHandle resource, const String& filePath, bool overwrite = false);

	public:
		struct ResourceMetaData : public IReflectable
		{
			String mUUID;
			String mPath;

			/************************************************************************/
			/* 								SERIALIZATION                      		*/
			/************************************************************************/
		public:
			friend class ResourceMetaDataRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

	private:
		typedef std::shared_ptr<ResourceMetaData> ResourceMetaDataPtr;
		map<String, ResourceMetaDataPtr>::type mResourceMetaData;
		map<String, ResourceMetaDataPtr>::type mResourceMetaData_FilePath;

		ResourceRequestHandler* mRequestHandler;
		ResourceResponseHandler* mResponseHandler;

		WorkQueuePtr mWorkQueue; // TODO Low priority - I might want to make this more global so other classes can use it
		UINT16 mWorkQueueChannel;

		unordered_map<String, BaseResourceHandle>::type mLoadedResources; // TODO Low priority - I'm not sure how will UUID (a string) do as key do performance wise
		unordered_map<String, ResourceAsyncOp>::type mInProgressResources; // Resources that are being asynchronously loaded

		BaseResourceHandle loadInternal(const String& filePath, bool synchronous); 
		ResourcePtr loadFromDiskAndDeserialize(const String& filePath);

		void loadMetaData();
		void saveMetaData(const ResourceMetaDataPtr metaData);

		void createMetaData(const String& uuid, const String& filePath);
		void addMetaData(const String& uuid, const String& filePath);
		void updateMetaData(const String& uuid, const String& newFilePath);
		void removeMetaData(const String& uuid);

		bool metaExists_UUID(const String& uuid) const;
		bool metaExists_Path(const String& path) const;

		const String& getPathFromUUID(const String& uuid) const;
		const String& getUUIDFromPath(const String& path) const;

		String mMetaDataFolderPath;

	private:
		friend class SpecificImporter;

		/**
		 * @brief	Registers the newly loaded resource in the global Resources library.
		 */
		void registerLoadedResource(BaseResourceHandle resource);
	};

	CM_EXPORT Resources& gResources();
}