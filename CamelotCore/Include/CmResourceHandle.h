#pragma once

#include "CmIReflectable.h"

namespace CamelotEngine
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
		void waitUntilLoaded() const;

		/**
		 * @brief	Returns the UUID of the resource the handle is referring to.
		 */
		const String& getUUID() const { return mData->mUUID; }

		/**
		 * @brief	Gets the handle data. For internal use only.
		 */
		std::shared_ptr<ResourceHandleData> getHandleData() const { return mData; }

	protected:
		ResourceHandleBase();

		std::shared_ptr<ResourceHandleData> mData;

		void init(Resource* ptr);
		void init(std::shared_ptr<Resource> ptr);

		template <typename T1>
		void init(const ResourceHandle<T1>& ptr)
		{
			mData = ptr.mData;
		}
	private:
		friend class Resources;

		CM_STATIC_THREAD_SYNCHRONISER(mResourceCreatedCondition)
		CM_STATIC_MUTEX(mResourceCreatedMutex)

		/**
		 * @brief	Sets the created flag to true. Should only be called
		 * 			by Resources class after loading of the resource is fully done.
		 */
		void resolve(std::shared_ptr<Resource> ptr);

		/**
		 * @brief	Sets an uuid of the ResourceHandle. Should only be called by
		 * 			Resources class.
		 */
		void ResourceHandleBase::setUUID(const String& uuid);

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
		{	}

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

		// TODO Low priority - User can currently try to access these even if resource ptr is not resolved
		T* get() const 
		{ 
			throwIfNotLoaded();

			return reinterpret_cast<T*>(mData->mPtr.get()); 
		}
		T* operator->() const { return get(); }
		T& operator*() const { return *get(); }

		std::shared_ptr<T> getInternalPtr() const
		{ 
			throwIfNotLoaded();

			return std::static_pointer_cast<T>(mData->mPtr); 
		}

		/**
		 * @brief	Releases the reference held by this handle.
		 */
		void reset()
		{
			mData = std::shared_ptr<ResourceHandleData>(new ResourceHandleData());
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
			return (((mData->mPtr != nullptr)) ? &CM_Bool_struct<T>::_Member : 0);
		}

	private:
		friend class Resource;

		explicit ResourceHandle(T* ptr)
			:ResourceHandleBase()
		{
			init(ptr);
		}

		ResourceHandle(std::shared_ptr<T> ptr)
			:ResourceHandleBase()
		{
			init(ptr);
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
		return (_Left.get() == _Right.get());
	}

	template<class _Ty1, class _Ty2>
	bool operator!=(const ResourceHandle<_Ty1>& _Left, const ResourceHandle<_Ty2>& _Right)
	{	
		return (!(_Left == _Right));
	}
}