#pragma once

#include "CmPrerequisites.h"
#include "CmVertexDeclaration.h"
#include "CmVertexBuffer.h"

namespace BansheeEngine 
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/

	/** Summary class collecting together vertex source information. */
	class CM_EXPORT VertexData
	{
    public:
        VertexData();
        ~VertexData();

		/** Declaration of the vertex to be used in this operation. 
		@remarks Note that this is created for you on construction.
		*/
		VertexDeclarationPtr vertexDeclaration;

		/// The number of vertices used in this operation
		UINT32 vertexCount;

		void setBuffer(UINT32 index, VertexBufferPtr buffer);
		/// Gets the buffer bound to the given source index
		VertexBufferPtr getBuffer(UINT32 index) const;
		const UnorderedMap<UINT32, VertexBufferPtr>& getBuffers() const { return mVertexBuffers; }
		/// Gets whether a buffer is bound to the given source index
		bool isBufferBound(UINT32 index) const;

		UINT32 getBufferCount(void) const { return (UINT32)mVertexBuffers.size(); }
		UINT32 getMaxBufferIndex(void) const { return (UINT32)mVertexBuffers.size(); }

    private:
		/** The vertex buffer bindings to be used. 
		@remarks Note that this is created for you on construction.
		*/
		UnorderedMap<UINT32, VertexBufferPtr> mVertexBuffers;
	};
}