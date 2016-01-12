#pragma once

#include "BsCorePrerequisites.h"
#include "BsHardwareBuffer.h"
#include "BsCoreObject.h"

namespace BansheeEngine 
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** Contains information about a vertex buffer buffer. */
	class BS_CORE_EXPORT VertexBufferProperties
	{
	public:
		VertexBufferProperties(UINT32 numVertices, UINT32 vertexSize);

		/**	Gets the size in bytes of a single vertex in this buffer. */
        UINT32 getVertexSize() const { return mVertexSize; }

		/**	Get the number of vertices in this buffer. */
        UINT32 getNumVertices() const { return mNumVertices; }

	protected:
		friend class VertexBuffer;
		friend class VertexBufferCore;

		UINT32 mNumVertices;
		UINT32 mVertexSize;
	};

	/** @cond INTERNAL */

	/** Core thread specific implementation of a VertexBuffer. */
	class BS_CORE_EXPORT VertexBufferCore : public CoreObjectCore, public HardwareBuffer
	{
	public:
		VertexBufferCore(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool streamOut);
		virtual ~VertexBufferCore() { }

		/**	Returns information about the vertex buffer. */
		const VertexBufferProperties& getProperties() const { return mProperties; }

	protected:
		VertexBufferProperties mProperties;
	};

	/** @endcond */

	/**	Specialization of a hardware buffer used for holding vertex data. */
    class BS_CORE_EXPORT VertexBuffer : public CoreObject
    {
	public:
		virtual ~VertexBuffer() { }

		/**
		 * Retrieves a core implementation of a vertex buffer usable only from the core thread.
		 *
		 * @note	Core thread only.
		 */
		SPtr<VertexBufferCore> getCore() const;

		/** @copydoc HardwareBufferManager::createVertexBuffer */
		static VertexBufferPtr create(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		static const int MAX_SEMANTIC_IDX = 8;
	protected:
		friend class HardwareBufferManager;

		VertexBuffer(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool streamOut = false);

		/** @copydoc CoreObject::createCore */
		virtual SPtr<CoreObjectCore> createCore() const;

	protected:
		VertexBufferProperties mProperties;
		GpuBufferUsage mUsage;
		bool mStreamOut;
    };

	/** @} */
}