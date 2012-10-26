#pragma once

#include "CmIReflectable.h"

namespace CamelotEngine
{
	template <typename T>
	class ResourceRef;

	class CM_EXPORT ResourceRefBase : public IReflectable
	{
	protected:
		ResourceRefBase()
			:mUUIDSet(false)
		{ }

		std::shared_ptr<Resource> mPtr;
		String mUUID;
		bool mUUIDSet;

		void init(Resource* ptr);
		void init(std::shared_ptr<Resource> ptr);

		template <typename T1>
		void init(const ResourceRef<T1>& ptr)
		{
			init(std::static_pointer_cast<Resource>(ptr.mPtr));
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
			return ResourceRef<Resource>(this->mPtr); 
		}

		const T* get() const { return static_cast<T*>(mPtr.get()); }
		const T* operator-> () const { return get(); }
		const T& operator* () const { return *get(); }

		T* get() { return static_cast<T*>(mPtr.get()); }
		T* operator-> () { return get(); }
		T& operator* () { return *get(); }

		std::shared_ptr<T> getInternalPtr() { return std::static_pointer_cast<T>(mPtr); }
	};

	template<class _Ty1, class _Ty2>
		ResourceRef<_Ty1> static_resource_cast(const ResourceRef<_Ty2>& other)
	{	
		return ResourceRef<_Ty1>(other);
	}
}