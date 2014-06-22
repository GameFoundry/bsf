#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Descriptor structure used for initializing a GPU buffer view.
	 *
	 * @see		GpuBufferView
	 * @see		GpuBuffer
	 */
	struct BS_CORE_EXPORT GPU_BUFFER_DESC
	{
		UINT32 firstElement;
		UINT32 elementWidth;
		UINT32 numElements;
		bool useCounter;
		GpuViewUsage usage;
	};

	/**
	 * @brief	Holds information about a GPU buffer view. Views allow you to specify
	 *			how is data in a buffer organized to make it easier for the pipeline to interpret.
	 *			
	 * @note	Buffers don't get bound to the pipeline directly, views do.
	 *			Core thread only.
	 *
	 * @see		GpuBuffer
	 */
	class BS_CORE_EXPORT GpuBufferView
	{
	public:
		class HashFunction
		{
		public:
			size_t operator()(const GPU_BUFFER_DESC& key) const;
		};

		class EqualFunction
		{
		public:
			bool operator()(const GPU_BUFFER_DESC& a, const GPU_BUFFER_DESC& b) const;
		};

		GpuBufferView();
		virtual ~GpuBufferView();

		/**
		 * @brief	Initializes the view with the specified buffer and 
		 *			a set of parameters describing the view to create.
		 *			Must be called right after construction.
		 */
		virtual void initialize(GpuBufferPtr buffer, GPU_BUFFER_DESC& desc);

		/**
		 * @brief	Returns a descriptor structure used for creating the view.
		 */
		const GPU_BUFFER_DESC& getDesc() const { return mDesc; }

		/**
		 * @brief	Returns the buffer this view was created for.
		 */
		GpuBufferPtr getBuffer() const { return mBuffer; }

		/**
		 * @brief	Returns index of first element in the buffer that this view
		 *			provides access to.
		 */
		UINT32 getFirstElement() const { return mDesc.firstElement; }

		/**
		 * @brief	Returns width of an element in the buffer, in bytes.
		 */
		UINT32 getElementWidth() const { return mDesc.elementWidth; }

		/**
		 * @brief	Returns the total number of elements this buffer provides
		 *			access to.
		 */
		UINT32 getNumElements() const { return mDesc.numElements; }

		/**
		 * @brief	Returns true if this view allows a GPU program to use counters on
		 *			the bound buffer.
		 */
		bool getUseCounter() const { return mDesc.useCounter; }

		/**
		 * @brief	Returns view usage which determines where in the pipeline 
		 *			can the view be bound.
		 */
		GpuViewUsage getUsage() const { return mDesc.usage; }

	protected:
		GPU_BUFFER_DESC mDesc;
		GpuBufferPtr mBuffer;
	};
}