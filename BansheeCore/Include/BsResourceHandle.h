#pragma once

#include "BsIReflectable.h"

namespace BansheeEngine
{
	template <typename T>
	class ResourceHandle;

	/**
	 * @brief	Data that is shared between all resource handles.
	 */
	struct BS_CORE_EXPORT ResourceHandleData
	{
		ResourceHandleData()
			:mIsCreated(false)
		{ }

		std::shared_ptr<Resource> mPtr;
		String mUUID;
		bool mIsCreated;	
	};

	/**
	 * @brief	Base class containing common functionality for resource handles.
	 */
	class BS_CORE_EXPORT ResourceHandleBase : public IReflectable
	{
	public:
		virtual ~ResourceHandleBase();

		/**
		 * @brief	Checks if the resource is loaded. Until resource is loaded this handle
		 *			is invalid and you may not get the internal resource from it.
		 */
		bool isLoaded() const;

		/**
		 * @brief	Blocks the current thread until the resource is fully loaded.
		 * 			
		 * @note	Careful not to call this on the thread that does the loading.
		 */
		void blockUntilLoaded() const;

		/**
		 * @brief	Returns the UUID of the resource the handle is referring to.
		 */
		const String& getUUID() const { return mData != nullptr ? mData->mUUID : StringUtil::BLANK; }

		/**
		 * @brief	Gets the handle data. For internal use only.
		 */
		const std::shared_ptr<ResourceHandleData>& getHandleData() const { return mData; }

		/**
		 * @brief	Sets the created flag to true and assigns the resource pointer. Called
		 * 			by the constructors, or if you constructed just using a UUID, then you need to
		 * 			call this manually before you can access the resource from this handle.
		 * 			
		 * @note	This is needed because two part construction is required due to 
		 *			multithreaded nature of resource loading.
		 *			Internal method.
		 */
		void _setHandleData(std::shared_ptr<Resource> ptr, const String& uuid);

		/**
		 * @brief	Sets the internal handle data to another previously created data.
		 *
		 * @note	Internal method.
		 */
		void _setHandleData(const std::shared_ptr<ResourceHandleData>& data);

	protected:
		ResourceHandleBase();

		/** @note	All handles to the same source must share this same handle data. Otherwise things
		 *			like counting number of references or replacing pointed to resource become impossible
		 *			without additional logic. */
		std::shared_ptr<ResourceHandleData> mData;

	private:
		friend class Resources;

		BS_STATIC_THREAD_SYNCHRONISER(mResourceCreatedCondition)
		BS_STATIC_MUTEX(mResourceCreatedMutex)

	protected:
		inline void throwIfNotLoaded() const;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ResourceHandleRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	/**
	 * @brief	Represents a handle to a resource. Handles are similar to a smart pointers, but they have two advantages:
	 *				- When loading a resource asynchronously you can be immediately returned the handle
	 *				  that you may use throughout the engine. The handle will be made valid as soon as
	 *				  the resource is loaded.
	 *				- Handles can be serialized and deserialized, therefore saving/restoring references
	 *				  to their original resource.
	 */
	template <typename T>
	class ResourceHandle : public ResourceHandleBase
	{
	public:
		ResourceHandle()
			:ResourceHandleBase()
		{ }

		/**
		 * @brief	Constructs an invalid handle with the specified UUID. You must call _setHandleData
		 *			with the actual resource pointer to make the handle valid.
		 */
		ResourceHandle(const String& uuid)
			:ResourceHandleBase()
		{
			mData = bs_shared_ptr<ResourceHandleData, PoolAlloc>();
			mData->mUUID = uuid;
		}

		/**
		 * @brief	Copy constructor.
		 */
		ResourceHandle(const ResourceHandle<T>& ptr)
			:ResourceHandleBase()
		{
			mData = ptr.getHandleData();
		}

		/**
		 * @brief	Converts a specific handle to generic Resource handle.
		 */
		operator ResourceHandle<Resource>() const
		{
			ResourceHandle<Resource> handle;
			handle._setHandleData(getHandleData());

			return handle;
		}

		/**
		 * @brief	Returns internal resource pointer.
		 *
		 * @note	Throws exception if handle is invalid.
		 */
		T* operator->() const { return get(); }

		/**
		 * @brief	Returns internal resource pointer and dereferences it.
		 *
		 * @note	Throws exception if handle is invalid.
		 */
		T& operator*() const { return *get(); }

		/**
		 * @brief	Clears the handle making it invalid and releases any references
		 *			held to the resource.
		 */
		ResourceHandle<T>& operator=(std::nullptr_t ptr)
		{ 	
			mData = nullptr;
			return *this;
		}

		template<class _Ty>
		struct Bool_struct
		{
			int _Member;
		};

		/**
		 * @brief	Allows direct conversion of handle to bool.
		 *
		 * @note	This is needed because we can't directly convert to bool 
		 *			since then we can assign pointer to bool and that's weird.
		 */
		operator int Bool_struct<T>::*() const
		{
			return ((mData != nullptr && mData->mPtr != nullptr) ? &Bool_struct<T>::_Member : 0);
		}

		/**
		 * @brief	Returns internal resource pointer and dereferences it.
		 *
		 * @note	Throws exception if handle is invalid.
		 */
		T* get() const 
		{ 
			throwIfNotLoaded();

			return reinterpret_cast<T*>(mData->mPtr.get()); 
		}

		/**
		 * @brief	Returns the internal shared pointer to the resource.
		 *
		 * @note	Throws exception if handle is invalid.
		 */
		std::shared_ptr<T> getInternalPtr() const
		{ 
			throwIfNotLoaded();

			return std::static_pointer_cast<T>(mData->mPtr); 
		}

	private:
		friend class Resources;

		/**
		 * @brief	Constructs a new valid handle for the provided resource with the provided UUID.
		 *			
		 * @note	Handle will take ownership of the provided resource pointer, so make sure you don't
		 *			delete it elsewhere.
		 */
		explicit ResourceHandle(T* ptr, const String& uuid)
			:ResourceHandleBase()
		{
			mData = bs_shared_ptr<ResourceHandleData, PoolAlloc>();
			_setHandleData(std::shared_ptr<Resource>(ptr, uuid));
		}

		/**
		 * @brief	Constructs a new valid handle for the provided resource with the provided UUID.
		 */
		ResourceHandle(std::shared_ptr<T> ptr, const String& uuid)
			:ResourceHandleBase()
		{
			mData = bs_shared_ptr<ResourceHandleData, PoolAlloc>();
			_setHandleData(ptr, uuid);
		}
	};

	/**
	 * @brief	Casts one resource handle to another.
	 */
	template<class _Ty1, class _Ty2>
		ResourceHandle<_Ty1> static_resource_cast(const ResourceHandle<_Ty2>& other)
	{	
		ResourceHandle<_Ty1> handle;
		handle._setHandleData(other.getHandleData());

		return handle;
	}

	/**
	 * @brief	Checks if two handles point to the same resource.
	 */
	template<class _Ty1, class _Ty2>
	bool operator==(const ResourceHandle<_Ty1>& _Left, const ResourceHandle<_Ty2>& _Right)
	{	
		if(_Left.getHandleData() != nullptr && _Right.getHandleData() != nullptr)
			return _Left.getHandleData()->mPtr == _Right.getHandleData()->mPtr;

		return _Left.getHandleData() == _Right.getHandleData();
	}

	template<class _Ty1, class _Ty2>
	bool operator!=(const ResourceHandle<_Ty1>& _Left, const ResourceHandle<_Ty2>& _Right)
	{	
		return (!(_Left == _Right));
	}
}