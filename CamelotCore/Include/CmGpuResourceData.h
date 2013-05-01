#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	/**
	 * @brief	You can use this class to read and write to various GPU resources. 
	 * 			
	 * @note	If you allocate an internal buffer to store the resource data, the ownership of the buffer
	 * 			will always remain with the initial instance of the class. If that initial instance
	 * 			is deleted, any potential copies will point to garbage data.
	 */
	class CM_EXPORT GpuResourceData : public IReflectable
	{
	public:
		GpuResourceData();
		GpuResourceData(const GpuResourceData& copy);
		virtual ~GpuResourceData();

		UINT8* getData() const;

		/**
		 * @brief	Allocates an internal buffer of a certain size. If there is another
		 * 			buffer already allocated, it will be freed and new one will be allocated.
		 *
		 * @param	size	The size of the buffer in bytes.
		 */
		void allocateInternalBuffer(UINT32 size);

		/**
		 * @brief	Frees the internal buffer that was allocated using "allocateInternal". Called automatically
		 * 			when the instance of the class is destroyed.
		 */
		void freeInternalBuffer();

		/**
		 * @brief	Makes the internal data pointer point to some external data. No copying is done, 
		 * 			so you must ensure that external data exists as long as this class uses it. You are also
		 * 			responsible for deleting the data when you are done with it.
		 *
		 * @note	If any internal data is allocated, it is freed.
		 */
		void setExternalBuffer(UINT8* data);

		/**
		 * @brief	Locks the data and makes it available only to the render thread. Don't call manually.
		 */
		void lock() const;

		/**
		 * @brief	Unlocks the data and makes it available to all threads. Don't call manually.
		 */
		void unlock() const;

	private:
		UINT8* mData;
		bool mOwnsData;
		mutable bool mLocked;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuResourceDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}