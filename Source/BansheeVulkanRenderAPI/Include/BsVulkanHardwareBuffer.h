//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsVulkanResource.h"
#include "BsHardwareBuffer.h"

namespace bs
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Wrapper around a Vulkan buffer object that manages its usage and lifetime. */
	class VulkanBuffer : public VulkanResource
	{
	public:
		/** 
		 * @param[in]	owner		Manager that takes care of tracking and releasing of this object.
		 * @param[in]	buffer		Actual low-level Vulkan buffer handle.
		 * @param[in]	view		Optional handle to the buffer view.
		 * @param[in]	memory		Memory mapped to the buffer.
		 * @param[in]	rowPitch	If buffer maps to an image sub-resource, length of a single row (in elements).
		 * @param[in]	slicePitch	If buffer maps to an image sub-resource, size of a single 2D surface (in elements).
		 */
		VulkanBuffer(VulkanResourceManager* owner, VkBuffer buffer, VkBufferView view, VkDeviceMemory memory, 
			UINT32 rowPitch = 0, UINT32 slicePitch = 0);
		~VulkanBuffer();

		/** Returns the internal handle to the Vulkan object. */
		VkBuffer getHandle() const { return mBuffer; }

		/** Returns a buffer view that covers the entire buffer. */
		VkBufferView getView() const { return mView; }

		/**
		 * If buffer represents an image sub-resource, this is the number of elements that separate one row of the 
		 * sub-resource from another (if no padding, it is equal to image width).
		 */
		UINT32 getRowPitch() const { return mRowPitch; }

		/**
		 * If buffer represents an image sub-resource, this is the number of elements that separate one column of the
		 * sub-resource from another (if no padding, it is equal to image height). Only relevant for 3D images.
		 */
		UINT32 getSliceHeight() const { return mSliceHeight; }

		/** 
		 * Returns a pointer to internal buffer memory. Must be followed by unmap(). Caller must ensure the buffer was
		 * created in CPU readable memory, and that buffer isn't currently being written to by the GPU.
		 */
		UINT8* map(VkDeviceSize offset, VkDeviceSize length) const;

		/** Unmaps a buffer previously mapped with map(). */
		void unmap();

		/** 
		 * Queues a command on the provided command buffer. The command copies the contents of the current buffer to
		 * the destination buffer. Caller must ensure the provided offsets and lengths are within valid bounds of
		 * both buffers.
		 */
		void copy(VulkanTransferBuffer* cb, VulkanBuffer* destination, VkDeviceSize srcOffset, VkDeviceSize dstOffset, 
			VkDeviceSize length);

		/** 
		 * Queues a command on the provided command buffer. The command copies the contents of the current buffer to
		 * the destination image subresource. 
		 */
		void copy(VulkanTransferBuffer* cb, VulkanImage* destination, const VkExtent3D& extent, 
			const VkImageSubresourceLayers& range, VkImageLayout layout);

	private:
		VkBuffer mBuffer;
		VkBufferView mView;
		VkDeviceMemory mMemory;

		UINT32 mRowPitch;
		UINT32 mSliceHeight;
	};
	
	/**	Class containing common functionality for all Vulkan hardware buffers. */
	class VulkanHardwareBuffer : public HardwareBuffer
	{
	public:
		/**	Available types of Vulkan buffers. */
		enum BufferType
		{
			/** Contains geometry vertices and their properties. */
			BT_VERTEX = 0x1,
			/** Contains triangle to vertex mapping. */
			BT_INDEX = 0x2,
			/** Contains GPU program parameters. */
			BT_UNIFORM = 0x4,
			/** Generic read-only GPU buffer containing formatted data. */
			BT_GENERIC = 0x8,
			/** Generic read/write GPU buffer containing formatted data. */
			BT_STORAGE = 0x10,
		};

		VulkanHardwareBuffer(BufferType type, GpuBufferFormat format, GpuBufferUsage usage, UINT32 size,
			GpuDeviceFlags deviceMask = GDF_DEFAULT);
		~VulkanHardwareBuffer();

		/** @copydoc HardwareBuffer::readData */
		void readData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx = 0, UINT32 queueIdx = 0) override;

		/** @copydoc HardwareBuffer::writeData */
		void writeData(UINT32 offset, UINT32 length, const void* source, 
			BufferWriteType writeFlags = BWT_NORMAL, UINT32 queueIdx = 0) override;

		/** @copydoc HardwareBuffer::copyData */
		void copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, 
			UINT32 length, bool discardWholeBuffer = false, UINT32 queueIdx = 0) override;

		/** 
		 * Gets the resource wrapping the buffer object, on the specified device. If hardware buffer device mask doesn't 
		 * include the provided device, null is returned. 
		 */
		VulkanBuffer* getResource(UINT32 deviceIdx) const { return mBuffers[deviceIdx]; }

	protected:
		/** @copydoc HardwareBuffer::map */
		void* map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx) override;

		/** @copydoc HardwareBuffer::unmap */
		void unmap() override;

		/** Creates a new buffer for the specified device, matching the current buffer properties. */
		VulkanBuffer* createBuffer(VulkanDevice& device, bool staging, bool readable);

		VulkanBuffer* mBuffers[BS_MAX_DEVICES];

		VulkanBuffer* mStagingBuffer;
		UINT32 mMappedDeviceIdx;
		UINT32 mMappedGlobalQueueIdx;
		UINT32 mMappedOffset;
		UINT32 mMappedSize;
		GpuLockOptions mMappedLockOptions;

		VkBufferCreateInfo mBufferCI;
		VkBufferViewCreateInfo mViewCI;
		VkBufferUsageFlags mUsageFlags;
		bool mDirectlyMappable : 1;
		bool mSupportsGPUWrites : 1;
		bool mRequiresView : 1;
		bool mReadable : 1;
		bool mIsMapped : 1;
	};

	/** @} */
}