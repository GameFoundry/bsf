#pragma once

#include "CmPrerequisites.h"
#include "CmCoreObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a GPU parameter block buffer. Parameter block buffers
	 *			are bound to GPU programs which then fetch parameters from those buffers.
	 *
	 *			Writing or reading from this buffer will translate directly to API calls
	 *			that update the GPU.
	 * 			
	 * @note	Core thread only.
	 */
	class CM_EXPORT GpuParamBlockBuffer : public CoreObject
	{
	public:
		GpuParamBlockBuffer();
		virtual ~GpuParamBlockBuffer();

		/**
		 * @brief	Initializes a buffer with the specified size in bytes and usage.
		 *			Specify dynamic usage if you plan on modifying the buffer often,
		 *			otherwise specify static usage.
		 *
		 * @see		CoreObject::initialize
		 * 
		 * @note	Must be called right after construction.
		 */
		void initialize(UINT32 size, GpuParamBlockUsage usage);

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

		/**
		 * @brief	Returns	a parameter block buffer which is used for caching 
		 *			the parameter information on the CPU. Essentially a CPU
		 *			copy of the GPU buffer.
		 *
		 * @note	Sim thread only.
		 */
		GpuParamBlock* getParamBlock() const { return mParamBlock; }

	protected:
		GpuParamBlockUsage mUsage;
		UINT32 mSize;

		GpuParamBlock* mParamBlock;
	};

	/**
	 * @brief	Implementation of a GpuParamBlock buffer that doesn't use a GPU buffer
	 *			for storage. Used with APIs that do not support GPU parameter buffers.
	 */
	class CM_EXPORT GenericGpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		/**
		 * @copydoc	GpuParamBlockBuffer::writeData
		 */
		void writeData(const UINT8* data);

		/**
		 * @copydoc GpuParamBlockBuffer::readData.
		 */
		void readData(UINT8* data) const;

	protected:
		UINT8* mData;

		/**
		 * @copydoc CoreObject::initialize_internal.
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc CoreObject::destroy_internal.
		 */
		virtual void destroy_internal();
	};
}