#pragma once

#include "BsCorePrerequisites.h"
#include "BsVertexDeclaration.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains information about layout of vertices in a buffer.
	 *			This is very similar to VertexDeclaration but unlike VertexDeclaration it has no
	 *			render API object to back it up and is very lightweight.
	 */
	class BS_CORE_EXPORT VertexDataDesc : public IReflectable
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
		 * @brief	Returns the size in bytes of the vertex element with the specified semantic.
		 */
		UINT32 getElementSize(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;

		/**
		 * @brief	Returns offset of the vertex from start of the stream in bytes.
		 */
		UINT32 getElementOffsetFromStream(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;

		/**
		 * @brief	Gets vertex stride in bytes (offset from one vertex to another) in the specified stream.
		 */
		UINT32 getVertexStride(UINT32 streamIdx) const;

		/**
		 * @brief	Gets vertex stride in bytes (offset from one vertex to another) in all the streams.
		 */
		UINT32 getVertexStride() const;

		/**
		 * @brief	Gets offset in bytes from the start of the internal buffer to the start of the specified stream.
		 */
		UINT32 getStreamOffset(UINT32 streamIdx) const;

		/**
		 * @brief	Returns the number of vertex elements.
		 */
		UINT32 getNumElements() const { return (UINT32)mVertexElements.size(); }

		/**
		 * @brief	Returns the vertex element at the specified index.
		 */
		const VertexElement& getElement(UINT32 idx) const { return mVertexElements[idx]; }

		/**
		 * @brief	Creates a list of vertex elements from internal data.
		 */
		List<VertexElement> createElements() const;

		/**
		 * @brief	Creates a new empty vertex data descriptor.
		 */
		static VertexDataDescPtr create();

	private:
		friend class Mesh;
		friend class MeshCore;
		friend class MeshHeap;
		friend class MeshHeapCore;

		/**
		 * @brief	Returns the largest stream index of all the stored vertex elements.
		 */
		UINT32 getMaxStreamIdx() const;

		/**
		 * @brief	Checks if any of the vertex elements use the specified stream index.
		 */
		bool hasStream(UINT32 streamIdx) const;

		/**
		 * @brief	Removes a vertex element of the specified type and semantics if it exists.
		 */
		void clearIfItExists(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx);

	private:
		Vector<VertexElement> mVertexElements;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class VertexDataDescRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}