#pragma once

#include "CmIReflectable.h"

namespace CamelotFramework
{
	template <typename T>
	class ResourceHandle;

	struct CM_EXPORT ResourceHandleData
	{
		ResourceHandleData()
			:mIsCreated(false)
		{ }

		std::shared_ptr<Resource> mPtr;
		String mUUID;
		bool mIsCreated;	
	};

	class CM_EXPORT ResourceHandleBase : public IReflectable
	{
	public:
		/**
		 * @brief	Checks if the resource is loaded
		 */
		bool isLoaded() const;

		/**
		 * @brief	Blocks the current thread until the resource is fully loaded.
		 * 			
		 * @note	Careful not to call this on the thread that does the loading.
		 */
		void synchronize() const;

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
		 * @note	Two set construction is sometimes required due to multithreaded nature of resource loading.
		 */
		void _setHandleData(std::shared_ptr<Resource> ptr, const String& uuid);

	protected:
		ResourceHandleBase();

		std::shared_ptr<ResourceHandleData> mData;

	private:
		friend class Resources;

		CM_STATIC_THREAD_SYNCHRONISER(mResourceCreatedCondition)
		CM_STATIC_MUTEX(mResourceCreatedMutex)

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

	template <typename T>
	class ResourceHandle : public ResourceHandleBase
	{
	public:
		ResourceHandle()
			:ResourceHandleBase()
		{ }

		// Note: This constructor requires you to call "setHandleData" with the actual resource pointer,
		// before the resource is considered as loaded
		ResourceHandle(const String& uuid)
			:ResourceHandleBase()
		{
			mData = cm_shared_ptr<ResourceHandleData, PoolAlloc>();
			mData->mUUID = uuid;
		}

		template <typename T1>
		ResourceHandle(const ResourceHandle<T1>& ptr)
			:ResourceHandleBase()
		{
			mData = ptr.getHandleData();
		}

		operator ResourceHandle<Resource>() 
		{
			return ResourceHandle<Resource>(*this); 
		}

		T* operator->() const { return get(); }
		T& operator*() const { return *get(); }

		ResourceHandle<T>& operator=(std::nullptr_t ptr)
		{ 	
			mData = nullptr;
			return *this;
		}

		template<class _Ty>
		struct CM_Bool_struct
		{
			int _Member;
		};

		// Conversion to bool
		// (Why not just directly convert to bool? Because then we can assign pointer to bool and that's weird)
		operator int CM_Bool_struct<T>::*() const
		{
			return ((mData != nullptr && mData->mPtr != nullptr) ? &CM_Bool_struct<T>::_Member : 0);
		}

		T* get() const 
		{ 
			throwIfNotLoaded();

			return reinterpret_cast<T*>(mData->mPtr.get()); 
		}

		std::shared_ptr<T> getInternalPtr() const
		{ 
			throwIfNotLoaded();

			return std::static_pointer_cast<T>(mData->mPtr); 
		}

	private:
		friend class Resources;

		explicit ResourceHandle(T* ptr, const String& uuid)
			:ResourceHandleBase()
		{
			mData = cm_shared_ptr<ResourceHandleData, PoolAlloc>();
			_setHandleData(std::shared_ptr<Resource>(ptr, uuid));
		}

		ResourceHandle(std::shared_ptr<T> ptr, const String& uuid)
			:ResourceHandleBase()
		{
			mData = cm_shared_ptr<ResourceHandleData, PoolAlloc>();
			_setHandleData(ptr, uuid);
		}
	};

	template<class _Ty1, class _Ty2>
		ResourceHandle<_Ty1> static_resource_cast(const ResourceHandle<_Ty2>& other)
	{	
		return ResourceHandle<_Ty1>(other);
	}

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