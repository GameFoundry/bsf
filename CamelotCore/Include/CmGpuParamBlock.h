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
	class CM_EXPORT GpuParamBlockBuffer
	{
	public:
		GpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage);
		virtual ~GpuParamBlockBuffer();

		/**
		 * @brief	Writes all of the data to the buffer.
		 * 			Data size must be the same size as the buffer;
		 */
		virtual void writeAll(const void* data);

		const UINT8* getDataPtr(UINT32 offset) const;
		UINT32 getSize() const { return mSize; }

	protected:
		GpuParamBlockUsage mUsage;
		UINT8* mData;
		UINT32 mSize;
	};

	class CM_EXPORT GpuParamBlock : public CoreObject
	{
	public:
		GpuParamBlock();
		virtual ~GpuParamBlock();

		void initialize(const GpuParamBlockDesc& desc, GpuParamBlockUsage usage);

		void write(UINT32 offset, const void* data, UINT32 size);
		void zeroOut(UINT32 offset, UINT32 size);

		/**
		 * @brief	Returns a buffer that may be bound as a material parameter.
		 */
		const GpuParamBlockBuffer* getBindableBuffer() const { return mBuffer; }

		/**
		 * @brief	Updates the internal buffer. You must call this if you want the buffer to be
		 * 			up to date after making changes to it. Buffer won't be touched if there were no changes
		 * 			so feel free to call this often.
		 * 			
		 * @note	This will only queue the buffer update on the render thread.
		 */
		void updateBuffer();

		static GpuParamBlockPtr create(const GpuParamBlockDesc& desc);
	protected:
		friend class HardwareBufferManager;

		GpuParamBlockBuffer* mBuffer;
		GpuParamBlockUsage mUsage;

		UINT8* mData;
		UINT32 mSize;
		bool mDirty;

		/**
		 * @copydoc CoreGpuObject::initialize_internal.
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc CoreGpuObject::destroy_internal.
		 */
		virtual void destroy_internal();

		/**
		 * @brief	Creates new GPU parameter buffer. Derived classes should
		 * 			return their own specific buffer implementations.
		 * 			
		 * @note	Should only be called from the render thread.
		 */
		virtual GpuParamBlockBuffer* createBuffer() const;

		void updateBuffer_internal(UINT8* data);
	};
}