#pragma once

#include "CmPrerequisitesUtil.h"

#if CM_DEBUG_MODE
#include "CmException.h"
#endif

namespace CamelotEngine
{
	/**
	 * @brief	A pointer that once locked will not allow its value to be accessed.
	 * 			
	 * @note	This is useful with multithreading where you need to pass along some value to a different
	 * 			thread, but want to ensure that no other thread can modify the value (without actually creating a copy
	 * 			of all the data).
	 * 			
	 *			LockedPtr MUST be the exclusive owner of the data it is pointing to. 
	 */
	template <class T>
	class LockedPtr
	{
	public:
		explicit LockedPtr(T* data)
			:mLocked(false)
		{
			mData = new std::shared_ptr<T>(data);
		}

		T* operator->() const 
		{ 
			throwIfLocked();
			return !mLocked ? mData.get() : nullptr; 
		}

		void lock() { mLocked = true; }

		template<class _Ty>
		struct CM_Bool_struct
		{
			int _Member;
		};

		// Conversion to bool
		// (Why not just directly convert to bool? Because then we can assign pointer to bool and that's weird)
		operator int CM_Bool_struct<T>::*() const
		{
			return ((mData.get() != 0) ? &CM_Bool_struct<T>::_Member : 0);
		}

	private:
		std::shared_ptr<T> mData;
		bool mLocked;

#if CM_DEBUG_MODE
		void throwIfLocked() const
		{
			if(mLocked)
			{
				CM_EXCEPT(InternalErrorException, "Attempting to access locked data!");
			}
		}
	};
#endif
}