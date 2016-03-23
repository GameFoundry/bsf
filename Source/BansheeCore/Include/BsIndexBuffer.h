//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsHardwareBuffer.h"
#include "BsCoreObject.h"

namespace BansheeEngine 
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/**	Type of the indices used, used for determining size. */
	enum IndexType 
	{
		IT_16BIT,
		IT_32BIT
	};

	/**	Contains information about an index buffer. */
	class BS_CORE_EXPORT IndexBufferProperties
	{
	public:
		IndexBufferProperties(IndexType idxType, UINT32 numIndexes);

		/**	Returns the type of indices stored. */
		IndexType getType() const { return mIndexType; }

		/**	Returns the number of indices this buffer can hold. */
		UINT32 getNumIndices() const { return mNumIndexes; }

		/**	Returns the size of a single index in bytes. */
		UINT32 getIndexSize() const { return mIndexSize; }

	protected:
		friend class IndexBuffer;
		friend class IndexBufferCore;

		IndexType mIndexType;
		UINT32 mNumIndexes;
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
		SPtr<IndexBufferCore> getCore() const;

		/** @copydoc HardwareBufferManager::createIndexBuffer */
		static IndexBufferPtr create(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

	protected:
		friend class HardwareBufferManager;

		IndexBuffer(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage);

		/** @copydoc CoreObject::createCore */
		virtual SPtr<CoreObjectCore> createCore() const;

		IndexBufferProperties mProperties;
		GpuBufferUsage mUsage;
    };

	/** @} */

	/** @addtogroup RenderAPI-Internal
	 *  @{
	 */

	/** Core thread specific implementation of an IndexBuffer. */
	class BS_CORE_EXPORT IndexBufferCore : public CoreObjectCore, public HardwareBuffer
	{
	public:
		IndexBufferCore(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage);
		virtual ~IndexBufferCore() { }

		/**	Returns information about the index buffer. */
		const IndexBufferProperties& getProperties() const { return mProperties; }

	protected:
		IndexBufferProperties mProperties;
	};

	/** @} */
}