#pragma once

#include "CmPrerequisites.h"
#include "CmVertexDeclaration.h"

namespace BansheeEngine
{
	class CM_EXPORT VertexDataDesc : public IReflectable
	{
	public:
		VertexDataDesc() {}

		/**
		* @brief	Informs the internal buffer that it needs to make room for the specified vertex element. If a vertex
		* 			with same stream and semantics already exists it will just be updated. 
		 *
		 * @param	type	   	Type of the vertex element. Determines size.
		 * @param	semantic   	Semantic that allows the engine to connect the data to a shader input slot.
		 * @param	semanticIdx	(optional) If there are multiple semantics with the same name, use different index to differentiate between them.
		 * @param	streamIdx  	(optional) Zero-based index of the stream. Each stream will internally be represented as a single vertex buffer.
		 */
		void addVertElem(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

		/**
		 * @brief	Query if we have vertex data for the specified semantic.
		 */
		bool hasElement(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;

		/**
		 * @brief	Creates a new vertex declaration based on set vertex elements.
		 */
		VertexDeclarationPtr createDeclaration() const;

		UINT32 getElementSize(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;
		UINT32 getElementOffsetFromStream(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;

		UINT32 getVertexStride(UINT32 streamIdx) const;
		UINT32 getVertexStride() const;

		UINT32 getStreamOffset(UINT32 streamIdx) const;

		UINT32 getNumElements() const { return (UINT32)mVertexElements.size(); }
		const VertexElement& getElement(UINT32 idx) const { return mVertexElements[idx]; }
	private:
		friend class Mesh; // To avoid polluting the public interface with a bunch of nearly useless methods for outside world
		friend class MeshHeap;

		Vector<VertexElement> mVertexElements;

		UINT32 getMaxStreamIdx() const;
		bool hasStream(UINT32 streamIdx) const;

		void clearIfItExists(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx);

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class VertexDataDescRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}