#pragma once

#include "CmIReflectable.h"

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
			:mUUIDSet(false)
		{ }

		std::shared_ptr<Resource> mPtr;
		String mUUID;
		bool mUUIDSet;

		friend class ResourceRefDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class CM_EXPORT ResourceRefBase : public IReflectable
	{
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