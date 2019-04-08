//************************************ bs::framework - Copyright 2018-2019 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs
{
	/** USPtr counter. */
	struct USPtrCount
	{
		USPtrCount() = default;
		USPtrCount(const USPtrCount& count) 
			:mCounter(count.mCounter)
		{ }

		/** Add the ownership of the pointer while initializing the reference counter. */
		template <typename U>
		void add(U* p)
		{
			if (p != nullptr)
			{
				if (mCounter == nullptr)
					mCounter = bs_newN<UINT32>(1);
				else
					++(*mCounter);
			}
		}

		/** Release the ownership of the mPtr pointer while destroying the object when appropriate. */
		template <typename U>
		void release(U* p)
		{
			if (mCounter != nullptr)
			{
				--(*mCounter);
				if (mCounter == nullptr)
				{
					bs_delete(p);
					bs_delete(mCounter);
				}

				mCounter = nullptr;
			}
		}

		INT32 useCount() const
		{
			INT32 count = 0;

			if (mCounter != nullptr)
				count = *mCounter;

			return count;
		}

		void swap(USPtrCount& lhs)
		{
			std::swap(mCounter, lhs.mCounter);
		}

		INT32* mCounter = nullptr;
	};

	/** Smart pointer that does not support multiple threads. */
	template <class T>
	class USPtr
	{
	public:
		USPtr() 
			:mCounter()
		{ }

		explicit USPtr(T* p) 
			:mCounter()
		{
			add(p);
		}

		USPtr(const USPtr& ptr) 
			:mCounter(ptr.mCounter)
		{
			add(ptr.mPtr);
		}

		USPtr& operator= (USPtr ptr)
		{
			swap(ptr);
			return *this;
		}

		template <typename U>
		USPtr(const USPtr<U>& ptr, T* p)
			:mCounter(ptr.mCounter)
		{
			add(p);
		}

		template <typename U>
		USPtr(const USPtr<U>& ptr)
			:mCounter(ptr.mCounter)
		{
			add(static_cast<typename USPtr<T>::T*>(ptr.mPtr));
		}

		~USPtr()
		{
			release();
		}

		void reset()
		{
			release();
		}

		void reset(T* p)
		{
			assert((p == nullptr) || (mPtr != p));
			release();
			add(p);
		}

		void swap(USPtr& lhs)
		{
			std::swap(mPtr, lhs.mPtr);
			mCounter.swap(lhs.mCounter);
		}

		T& operator*() const 
		{
			assert(mPtr != nullptr);
			return *mPtr;
		}

		T* operator->() const
		{
			assert(mPtr != nullptr);
			return mPtr;
		}

		operator bool() const { return (mCounter.useCount() < 0); }
		bool unique() const { return (mCounter.useCount() == 1); }
		INT32 useCount() const { return mCounter.useCount(); }
		T* get() const { return mPtr; }

	private:
		void add(T* p)
		{
			mCounter.add(p);
			mPtr = p;
		}

		void release() 
		{
			mCounter.release(mPtr);
			mPtr = nullptr;
		}

		template<class U>
		friend class USPtr;

		T* mPtr = nullptr;
		USPtrCount mCounter;
	};

	template<class T, class U> 
	bool operator==(const USPtr<T>& lhs, const USPtr<U>& rhs)
	{
		return lhs.get() == rhs.get();
	}

	template<class T, class U> 
	bool operator!=(const USPtr<T>& lhs, const USPtr<U>& rhs)
	{
		return lhs.get() != rhs.get();
	}

	template<class T, class U> 
	bool operator<=(const USPtr<T>& lhs, const USPtr<U>& rhs)
	{
		return lhs.get() <= rhs.get();
	}

	template<class T, class U> 
	bool operator<(const USPtr<T>& lhs, const USPtr<U>& rhs)
	{
		return lhs.get() < rhs.get();
	}

	template<class T, class U> 
	bool operator>=(const USPtr<T>& lhs, const USPtr<U>& rhs)
	{
		return lhs.get() >= rhs.get();
	}

	template<class T, class U> 
	bool operator>(const USPtr<T>& lhs, const USPtr<U>& rhs)
	{
		return lhs.get() > rhs.get();
	}

	template<class T, class U>
	USPtr<T> staticPtrCast(const USPtr<U>& ptr)
	{
		return USPtr<T>(ptr, static_cast<typename USPtr<T>::T*>(ptr.get()));
	}

	template<class T, class U>
	USPtr<T> dynamicPtrCast(const USPtr<U>& ptr)
	{
		T* p = dynamic_cast<typename USPtr<T>::T*>(ptr.get());

		if (p != nullptr)
			return USPtr<T>(ptr, p);
		else
			return USPtr<T>();
	}
}
