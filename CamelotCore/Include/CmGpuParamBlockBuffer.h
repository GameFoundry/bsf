#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmCoreObject.h"

namespace CamelotFramework
{
	/**
	 * @brief	Represents an actual GPU buffer. 
	 * 			Should only be accessed directly from render thread.
	 */
	class CM_EXPORT GpuParamBlockBuffer : public CoreObject
	{
	public:
		GpuParamBlockBuffer();
		virtual ~GpuParamBlockBuffer();

		void initialize(UINT32 size, GpuParamBlockUsage usage);

		/**
		 * @brief	Writes all of the data to the buffer.
		 * 			Data size must be the same size as the buffer;
		 */
		virtual void writeData(const UINT8* data) = 0;

		/**
		 * @brief	Copies data from the internal buffer to a pre-allocated array. 
		 * 			Be aware this generally isn't a very fast operation.
		 *
		 * @param [in,out]	data	Array where the data will be written to. Must be of
		 * 							"getSize()" bytes.
		 */
		virtual void readData(UINT8* data) const = 0;

		UINT32 getSize() const { return mSize; }

	protected:
		GpuParamBlockUsage mUsage;
		UINT32 mSize;
	};

	class CM_EXPORT GenericGpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		/**
		 * @brief	Writes all of the data to the buffer.
		 * 			Data size must be the same size as the buffer;
		 */
		void writeData(const UINT8* data);

		/**
		 * @copydoc GpuParamBlockBuffer::readAll.
		 */
		void readData(UINT8* data) const;

	protected:
		UINT8* mData;

		/**
		 * @copydoc CoreGpuObject::initialize_internal.
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc CoreGpuObject::destroy_internal.
		 */
		virtual void destroy_internal();
	};
}