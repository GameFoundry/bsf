//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "RenderAPI/BsHardwareBuffer.h"
#include "CoreThread/BsCoreObject.h"

namespace bs 
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** Descriptor structure used for initialization of an IndexBuffer. */
	struct INDEX_BUFFER_DESC
	{
		IndexType indexType; /**< Index type, determines the size of a single index. */
		UINT32 numIndices; /**< Number of indices can buffer can hold. */
		GpuBufferUsage usage = GBU_STATIC; /**< Usage that tells the hardware how will be buffer be used. */
	};

	/**	Contains information about an index buffer. */
	class BS_CORE_EXPORT IndexBufferProperties
	{
	public:
		IndexBufferProperties(IndexType idxType, UINT32 numIndexes);

		/**	Returns the type of indices stored. */
		IndexType getType() const { return mIndexType; }

		/**	Returns the number of indices this buffer can hold. */
		UINT32 getNumIndices() const { return mNumIndices; }

		/**	Returns the size of a single index in bytes. */
		UINT32 getIndexSize() const { return mIndexSize; }

	protected:
		friend class IndexBuffer;
		friend class ct::IndexBuffer;

		IndexType mIndexType;
		UINT32 mNumIndices;
		UINT32 mIndexSize;
	};

	/** Hardware buffer that hold indices that reference vertices in a vertex buffer. */
    class BS_CORE_EXPORT IndexBuffer : public CoreObject
    {
	public:
		virtual ~IndexBuffer() { }

		/** Returns information about the index buffer. */
		const IndexBufferProperties& getProperties() const { return mProperties; }

		/**
		 * Retrieves a core implementation of an index buffer usable only from the core thread.
		 *
		 * @note	Core thread only.
		 */
		SPtr<ct::IndexBuffer> getCore() const;

		/** @copydoc HardwareBufferManager::createIndexBuffer */
		static SPtr<IndexBuffer> create(const INDEX_BUFFER_DESC& desc);

	protected:
		friend class HardwareBufferManager;

		IndexBuffer(const INDEX_BUFFER_DESC& desc);

		/** @copydoc CoreObject::createCore */
		virtual SPtr<ct::CoreObject> createCore() const;

		IndexBufferProperties mProperties;
		GpuBufferUsage mUsage;
    };

	/** @} */

	namespace ct
	{
	/** @addtogroup RenderAPI-Internal
	 *  @{
	 */

	/** Core thread specific implementation of an bs::IndexBuffer. */
	class BS_CORE_EXPORT IndexBuffer : public CoreObject, public HardwareBuffer
	{
	public:
		IndexBuffer(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);
		virtual ~IndexBuffer() { }

		/**	Returns information about the index buffer. */
		const IndexBufferProperties& getProperties() const { return mProperties; }

		/** @copydoc HardwareBufferManager::createIndexBuffer */
		static SPtr<IndexBuffer> create(const INDEX_BUFFER_DESC& desc, GpuDeviceFlags deviceMask = GDF_DEFAULT);

	protected:
		IndexBufferProperties mProperties;
	};

	/** @} */
		}
}