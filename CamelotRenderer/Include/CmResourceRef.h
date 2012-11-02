#pragma once

#include "CmIReflectable.h"
#include <atomic>

template<class _Ty>
struct CM_Bool_struct
{
	int _Member;
};

namespace CamelotEngine
{
	template <typename T>
	class ResourceRef;

	struct CM_EXPORT ResourceRefData : public IReflectable
	{
		ResourceRefData()
		{
			mIsResolved.store(false);
		}

		std::shared_ptr<Resource> mPtr;
		String mUUID;
		std::atomic_bool mIsResolved; // TODO Low priority. I might not need an atomic here. Volatile bool should do.

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ResourceRefDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;		
	};

	class CM_EXPORT ResourceRefBase : public IReflectable
	{
	public:
		/**
		 * @brief	Checks if the resource is loaded
		 */
		bool isResolved() { return mData->mIsResolved.load(); }

	protected:
		ResourceRefBase();

		std::shared_ptr<ResourceRefData> mData;

		void init(Resource* ptr);
		void init(std::shared_ptr<Resource> ptr);

		template <typename T1>
		void init(const ResourceRef<T1>& ptr)
		{
			init(std::static_pointer_cast<Resource>(ptr.mData->mPtr));
		}
	private:
		friend class Resources;
		/**
		 * @brief	Sets the resolved flag to true. Should only be called
		 * 			by Resources class after loading of the resource is fully done.
		 */
		void resolve(std::shared_ptr<Resource> ptr);

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ResourceRefRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	template <typename T>
	class ResourceRef : public ResourceRefBase
	{
	public:
		ResourceRef()
			:ResourceRefBase()
		{	}

		ResourceRef(T* ptr)
			:ResourceRefBase()
		{
			init(ptr);
		}

		ResourceRef(std::shared_ptr<T> ptr)
			:ResourceRefBase()
		{
			init(ptr);
		}

		template <typename T1>
		ResourceRef(const ResourceRef<T1>& ptr)
			:ResourceRefBase()
		{
			init(ptr);
		}

		operator ResourceRef<Resource>() 
		{
			return ResourceRef<Resource>(mData->mPtr); 
		}

		// TODO Low priority - User can currently try to access these even if resource ptr is not resolved
		T* get() const { return static_cast<T*>(mData->mPtr.get()); }
		T* operator->() const { return get(); }
		T& operator*() const { return *get(); }

		std::shared_ptr<T> getInternalPtr() { return std::static_pointer_cast<T>(mData->mPtr); }

		// Conversion to bool
		// (Why not just directly convert to bool? Because then we can assign pointer to bool and that's weird)
		operator int CM_Bool_struct<T>::*() const
		{
			return (mData->mPtr.get() != 0 ? &CM_Bool_struct<T>::_Member : 0);
		}
	};

	template<class _Ty1, class _Ty2>
		ResourceRef<_Ty1> static_resource_cast(const ResourceRef<_Ty2>& other)
	{	
		return ResourceRef<_Ty1>(other);
	}

	template<class _Ty1, class _Ty2>
	bool operator==(const ResourceRef<_Ty1>& _Left, const ResourceRef<_Ty2>& _Right)
	{	
		return (_Left.get() == _Right.get());
	}

	template<class _Ty1, class _Ty2>
	bool operator!=(const ResourceRef<_Ty1>& _Left, const ResourceRef<_Ty2>& _Right)
	{	
		return (!(_Left == _Right));
	}
}