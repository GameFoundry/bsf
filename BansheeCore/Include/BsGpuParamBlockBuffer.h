#pragma once

#include "BsCorePrerequisites.h"
#include "BsCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Core thread version of a GPU param block buffer.
	 *
	 * @see		GpuParamBlockBuffer
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT GpuParamBlockBufferCore : public CoreObjectCore
	{
	public:
		GpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage);
		virtual ~GpuParamBlockBufferCore() { }

		/**
		 * @brief	Writes all of the specified data to the buffer.
		 * 			Data size must be the same size as the buffer;
		 */
		virtual void writeData(const UINT8* data) = 0;

		/**
		 * @brief	Copies data from the internal buffer to a pre-allocated array. 
		 * 			Be aware this generally isn't a very fast operation as reading
		 *			from the GPU will most definitely involve a CPU-GPU sync point.
		 *
		 * @param [in,out]	data	Array where the data will be written to. Must be of
		 * 							"getSize()" bytes.
		 */
		virtual void readData(UINT8* data) const = 0;

		/**
		 * @brief	Returns the size of the buffer in bytes.
		 */
		UINT32 getSize() const { return mSize; }

	protected:
		GpuParamBlockUsage mUsage;
		UINT32 mSize;
	};

	/**
	 * @brief	Represents a GPU parameter block buffer. Parameter block buffers
	 *			are bound to GPU programs which then fetch parameters from those buffers.
	 *
	 *			Writing or reading from this buffer will translate directly to API calls
	 *			that update the GPU.
	 * 			
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT GpuParamBlockBuffer : public CoreObject
	{
	public:
		GpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage);
		virtual ~GpuParamBlockBuffer() { }

		/**
		 * @brief	Returns the size of the buffer in bytes.
		 */
		UINT32 getSize() const { return mSize; }

		/**
		 * @brief	Returns	a parameter block buffer which is used for caching 
		 *			the parameter information. Essentially a CPU copy of the GPU buffer.
		 *
		 * @note	Thread safe but it's up to the caller to ensure this is only called from
		 *			one thread.
		 */
		GpuParamBlockPtr getParamBlock() const { return mParamBlock; }

		/**
		 * @brief	Retrieves a core implementation of a GPU param block buffer usable only from the
		 *			core thread.
		 */
		SPtr<GpuParamBlockBufferCore> getCore() const;

		/**
		 * @copydoc	HardwareBufferManager::createGpuParamBlockBuffer
		 */
		static GpuParamBlockBufferPtr create(UINT32 size, GpuParamBlockUsage usage = GPBU_DYNAMIC);

	protected:
		/**
		 * @copydoc	CoreObject::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		GpuParamBlockUsage mUsage;
		UINT32 mSize;

		GpuParamBlockPtr mParamBlock;
	};

	/**
	 * @brief	Implementation of a GpuParamBlock buffer that doesn't use a GPU buffer
	 *			for storage. Used with APIs that do not support GPU parameter buffers.
	 */
	class BS_CORE_EXPORT GenericGpuParamBlockBufferCore : public GpuParamBlockBufferCore
	{
	public:
		GenericGpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage);

		/**
		 * @copydoc	GpuParamBlockBufferCore::writeData
		 */
		void writeData(const UINT8* data);

		/**
		 * @copydoc GpuParamBlockBufferCore::readData.
		 */
		void readData(UINT8* data) const;

	protected:
		UINT8* mData;

		/**
		 * @copydoc CoreObjectCore::initialize
		 */
		virtual void initialize();

		/**
		 * @copydoc CoreObjectCore::destroy
		 */
		virtual void destroy();
	};
}