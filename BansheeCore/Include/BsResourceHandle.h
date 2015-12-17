#pragma once

#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Data that is shared between all resource handles.
	 */
	struct BS_CORE_EXPORT ResourceHandleData
	{
		ResourceHandleData()
			:mIsCreated(false), mRefCount(0)
		{ }

		SPtr<Resource> mPtr;
		String mUUID;
		bool mIsCreated;	
		UINT32 mRefCount;
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
		 *
		 * @param	checkDependencies	If true, and if resource has any dependencies, this method will
		 *								also check if they are loaded.
		 */
		bool isLoaded(bool checkDependencies = true) const;

		/**
		 * @brief	Blocks the current thread until the resource is fully loaded.
		 * 			
		 * @note	Careful not to call this on the thread that does the loading.
		 */
		void blockUntilLoaded(bool waitForDependencies = true) const;

		/**
		 * @brief	Releases an internal reference to this resource held by the resources system, if there is one.
		 * 			
		 * @see		Resources::release(ResourceHandleBase&)
		 */
		void release();

		/**
		 * @brief	Returns the UUID of the resource the handle is referring to.
		 */
		const String& getUUID() const { return mData != nullptr ? mData->mUUID : StringUtil::BLANK; }

		/**
		 * @brief	Gets the handle data. For internal use only.
		 */
		const SPtr<ResourceHandleData>& getHandleData() const { return mData; }

	protected:
		ResourceHandleBase();

		/**
		 * @brief	Destroys the resource the handle is pointing to.
		 */
		void destroy();

		/**
		 * @brief	Sets the created flag to true and assigns the resource pointer. Called
		 * 			by the constructors, or if you constructed just using a UUID, then you need to
		 * 			call this manually before you can access the resource from this handle.
		 * 			
		 * @note	This is needed because two part construction is required due to 
		 *			multithreaded nature of resource loading.
		 *			Internal method.
		 */
		void setHandleData(const SPtr<Resource>& ptr, const String& uuid);

		/**
		 * @brief	Increments the reference count of the handle. Only to be used by ::Resources for keeping
		 * 			internal references.
		 */
		void addInternalRef();

		/**
		 * @brief	Decrements the reference count of the handle. Only to be used by ::Resources for keeping
		 * 			internal references.
		 */
		void removeInternalRef();

		/** @note	All handles to the same source must share this same handle data. Otherwise things
		 *			like counting number of references or replacing pointed to resource become impossible
		 *			without additional logic. */
		SPtr<ResourceHandleData> mData;

	private:
		friend class Resources;

		BS_STATIC_THREAD_SYNCHRONISER(mResourceCreatedCondition)
		BS_STATIC_MUTEX(mResourceCreatedMutex)

	protected:
		inline void throwIfNotLoaded() const;
	};

	/**
	 * @copydoc	ResourceHandleBase
	 *
	 * Handles differences in reference counting depending if the handle is normal or weak.
	 */
	template <bool WeakHandle>
	class BS_CORE_EXPORT TResourceHandleBase : public ResourceHandleBase { };

	/**
	 * @brief	Specialization of TResourceHandleBase for weak handles. Weak handles do no reference counting.
	 */
	template<>
	class BS_CORE_EXPORT TResourceHandleBase<true> : public ResourceHandleBase
	{
	public:
		virtual ~TResourceHandleBase() { }

	protected:
		void addRef() { };
		void releaseRef() { };

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class WeakResourceHandleRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Specialization of TResourceHandleBase for normal (non-weak) handles.
	 */
	template<>
	class BS_CORE_EXPORT TResourceHandleBase<false> : public ResourceHandleBase
	{
	public:
		virtual ~TResourceHandleBase() { }

	protected:
		void addRef() { if (mData) mData->mRefCount++; };
		void releaseRef() 
		{ 
			if (mData)
			{
				mData->mRefCount--;

				if (mData->mRefCount == 0)
					destroy();
			}
		};

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class WeakResourceHandleRTTI;
		friend class ResourceHandleRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};

	/**
	 * @brief	Represents a handle to a resource. Handles are similar to a smart pointers, but they have two advantages:
	 *				- When loading a resource asynchronously you can be immediately returned the handle
	 *				  that you may use throughout the engine. The handle will be made valid as soon as
	 *				  the resource is loaded.
	 *				- Handles can be serialized and deserialized, therefore saving/restoring references
	 *				  to their original resource.
	 */
	template <typename T, bool WeakHandle>
	class TResourceHandle : public TResourceHandleBase<WeakHandle>
	{
	public:
		TResourceHandle()
			:TResourceHandleBase()
		{ }

		/**
		 * @brief	Copy constructor.
		 */
		TResourceHandle(const TResourceHandle<T, WeakHandle>& ptr)
			:TResourceHandleBase()
		{
			mData = ptr.getHandleData();

			addRef();
		}

		virtual ~TResourceHandle()
		{
			releaseRef();
		}

		/**
		 * @brief	Converts a specific handle to generic Resource handle.
		 */
		operator TResourceHandle<Resource, WeakHandle>() const
		{
			TResourceHandle<Resource, WeakHandle> handle;
			handle.setHandleData(getHandleData());

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
		TResourceHandle<T, WeakHandle>& operator=(std::nullptr_t ptr)
		{ 	
			releaseRef();

			mData = nullptr;
			return *this;
		}

		/**
		 * @brief	Normal assignment operator.
		 */
		TResourceHandle<T, WeakHandle>& operator=(const TResourceHandle<T, WeakHandle>& rhs)
		{ 	
			setHandleData(rhs.getHandleData());
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
			return ((mData != nullptr && !mData->mUUID.empty()) ? &Bool_struct<T>::_Member : 0);
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
		SPtr<T> getInternalPtr() const
		{ 
			throwIfNotLoaded();

			return std::static_pointer_cast<T>(mData->mPtr); 
		}

		/**
		 * @brief	Converts a handle into a weak handle.
		 */
		TResourceHandle<T, true> getWeak() const
		{
			TResourceHandle<T, true> handle;
			handle.setHandleData(getHandleData());

			return handle;
		}

	protected:
		friend Resources;
		template<class _T, bool _Weak>
		friend class TResourceHandle;
		template<class _Ty1, class _Ty2, bool Weak>
		friend TResourceHandle<_Ty1, Weak> static_resource_cast(const TResourceHandle<_Ty2, Weak>& other);

		/**
		 * @brief	Constructs a new valid handle for the provided resource with the provided UUID.
		 *			
		 * @note	Handle will take ownership of the provided resource pointer, so make sure you don't
		 *			delete it elsewhere.
		 */
		explicit TResourceHandle(T* ptr, const String& uuid)
			:TResourceHandleBase()
		{
			mData = bs_shared_ptr_new<ResourceHandleData>();
			addRef();

			setHandleData(std::shared_ptr<Resource>(ptr, uuid));
		}

		/**
		 * @brief	Constructs an invalid handle with the specified UUID. You must call setHandleData
		 *			with the actual resource pointer to make the handle valid.
		 */
		TResourceHandle(const String& uuid)
			:TResourceHandleBase()
		{
			mData = bs_shared_ptr_new<ResourceHandleData>();
			mData->mUUID = uuid;

			addRef();
		}

		/**
		 * @brief	Constructs a new valid handle for the provided resource with the provided UUID.
		 */
		TResourceHandle(const SPtr<T> ptr, const String& uuid)
			:TResourceHandleBase()
		{
			mData = bs_shared_ptr_new<ResourceHandleData>();
			addRef();

			setHandleData(ptr, uuid);
		}

		/**
		 * @brief	Replaces the internal handle data pointer, effectively transforming the handle into a different handle.
		 */
		void setHandleData(const SPtr<ResourceHandleData>& data)
		{
			releaseRef();
			mData = data;
			addRef();
		}

		/**
		 * @brief	Converts a weak handle into a normal handle.
		 */
		TResourceHandle<T, false> lock() const
		{
			TResourceHandle<Resource, false> handle;
			handle.setHandleData(getHandleData());

			return handle;
		}

		using TResourceHandleBase::setHandleData;
	};

	template <typename T>
	using ResourceHandle = TResourceHandle<T, false>;

	template <typename T>
	using WeakResourceHandle = TResourceHandle<T, true>;
	
	/**
	 * @brief	Casts one resource handle to another.
	 */
	template<class _Ty1, class _Ty2, bool Weak>
	TResourceHandle<_Ty1, Weak> static_resource_cast(const TResourceHandle<_Ty2, Weak>& other)
	{	
		TResourceHandle<_Ty1, Weak> handle;
		handle.setHandleData(other.getHandleData());

		return handle;
	}

	/**
	 * @brief	Checks if two handles point to the same resource.
	 */
	template<class _Ty1, bool _Weak1, class _Ty2, bool _Weak2>
	bool operator==(const TResourceHandle<_Ty1, _Weak1>& _Left, const TResourceHandle<_Ty2, _Weak2>& _Right)
	{	
		if(_Left.getHandleData() != nullptr && _Right.getHandleData() != nullptr)
			return _Left.getHandleData()->mPtr == _Right.getHandleData()->mPtr;

		return _Left.getHandleData() == _Right.getHandleData();
	}

	/**
	 * @brief	Checks if a handle is null.
	 */
	template<class _Ty1, bool _Weak1, class _Ty2, bool _Weak2>
	bool operator==(const TResourceHandle<_Ty1, _Weak1>& _Left, std::nullptr_t  _Right)
	{	
		return _Left.getHandleData() == nullptr || _Left.setHandleData().UUID.empty();
	}

	template<class _Ty1, bool _Weak1, class _Ty2, bool _Weak2>
	bool operator!=(const TResourceHandle<_Ty1, _Weak1>& _Left, const TResourceHandle<_Ty2, _Weak2>& _Right)
	{	
		return (!(_Left == _Right));
	}
}