#pragma once

#include "BsCorePrerequisites.h"
#include "BsVertexDeclaration.h"
#include "BsVertexBuffer.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Container class consisting of a set of vertex buffers and their
	 *			declaration.
	 *
	 * @note	Used just for more easily passing around vertex information.
	 */
	class BS_CORE_EXPORT VertexData
	{
    public:
        VertexData();
        ~VertexData();

		/**
		 * @brief	Declaration used for the contained vertex buffers.
		 */
		VertexDeclarationPtr vertexDeclaration;

		/**
		 * @brief	Number of vertices to use.
		 */
		UINT32 vertexCount;

		/**
		 * @brief	Assigns a new vertex buffer to the specified index.
		 */
		void setBuffer(UINT32 index, SPtr<VertexBufferCore> buffer);

		/**
		 * @brief	Retrieves a vertex buffer from the specified index.
		 */
		SPtr<VertexBufferCore> getBuffer(UINT32 index) const;

		/**
		 * @brief	Returns a list of all bound vertex buffers.
		 */
		const UnorderedMap<UINT32, SPtr<VertexBufferCore>>& getBuffers() const { return mVertexBuffers; }

		/**
		 * @brief	Checks if there is a buffer at the specified index.
		 */
		bool isBufferBound(UINT32 index) const;

		/**
		 * @brief	Gets total number of bound buffers.
		 */
		UINT32 getBufferCount() const { return (UINT32)mVertexBuffers.size(); }

		/**
		 * @brief	Returns the maximum index of all bound buffers.
		 */
		UINT32 getMaxBufferIndex() const { return mMaxBufferIdx; }

	private:
		void recalculateMaxIndex();

    private:
		UnorderedMap<UINT32, SPtr<VertexBufferCore>> mVertexBuffers;

		UINT32 mMaxBufferIdx = 0;
	};
}