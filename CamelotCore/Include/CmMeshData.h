#pragma once

#include "CmPrerequisites.h"
#include "CmGpuResourceData.h"
#include "CmVertexBuffer.h"
#include "CmIndexBuffer.h"
#include "CmVertexDeclaration.h"
#include "CmDrawOps.h"

namespace CamelotFramework
{
	template<class T>
	class VertexElemIter
	{
	public:
		VertexElemIter()
			:mData(nullptr), mEnd(nullptr), mByteStride(0), mNumElements(0)
		{

		}

		VertexElemIter(UINT8* data, UINT32 byteStride, UINT32 numElements)
			:mData(data), mByteStride(byteStride), mNumElements(numElements)
		{
			mEnd = mData + byteStride * numElements;
		}

		void addValue(T& value)
		{
			setValue(value);
			moveNext();
		}

		void setValue(T& value)
		{
			memcpy(mData, &value, sizeof(T));
		}

		T& getValue()
		{
			return *((T*)mData);
		}

		void moveNext()
		{
#ifdef CM_DEBUG_MODE
			if(mData >= mEnd)
			{
				CM_EXCEPT(InternalErrorException, "Vertex element iterator out of buffer bounds.");
			}
#endif

			mData += mByteStride;
		}

		UINT32 getNumElements() const { return mNumElements; }

	private:
		UINT8* mData;
		UINT8* mEnd;
		UINT32 mByteStride;
		UINT32 mNumElements;
	};

	class CM_EXPORT MeshData : public GpuResourceData
	{
	public:
		struct IndexElementData
		{
			IndexElementData()
				:numIndices(0), subMesh(0), elementSize(0), drawOp(DOT_TRIANGLE_LIST)
			{ }

			UINT32 numIndices;
			UINT32 elementSize;
			UINT32 subMesh;
			DrawOperationType drawOp;
		};

		MeshData(UINT32 numVertices, UINT32 numIndexes, const VertexDataDescPtr& vertexData, DrawOperationType drawOp = DOT_TRIANGLE_LIST, IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);
		~MeshData();

		void setVertexWriteOffset(UINT32 offset); // TODO
		void setIndexWriteOffset(UINT32 offset); // TODO

		/**
		 * @brief	Informs the internal buffer that it needs to make room for an index buffer of the
		 * 			specified size. If specified submesh already exists it will just be updated. This
		 * 			must be called between beginDesc and endDesc.
		 *
		 * @param	numIndices	Number of indices.
		 * @param	subMesh   	(optional) Index of the sub-mesh to add/update.
		 * @param	drawOp	  	(optional) Specifies the primitive type contained by the mesh.
		 */
		void addSubMesh(UINT32 numIndices, UINT32 subMesh = 0, DrawOperationType drawOp = DOT_TRIANGLE_LIST);

		/**
		 * @brief	Copies data from "data" parameter into the internal buffer for the specified semantic.
		 *
		 * @param	semantic   		Semantic that allows the engine to connect the data to a shader input slot.
		 * @param	data			Vertex data, containing at least "size" bytes.
		 * @param	size			The size of the data. Must be the size of the vertex element type * number of vertices.
		 * @param	semanticIdx 	(optional) If there are multiple semantics with the same name, use different index to differentiate between them.
		 * @param	streamIdx   	(optional) Zero-based index of the stream. Each stream will internally be represented as a single vertex buffer.
		 */
		void setVertexData(VertexElementSemantic semantic, UINT8* data, UINT32 size, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

		/**
		 * @brief	Returns an iterator you can use for easily retrieving or setting Vector2 vertex elements. This is the preferred
		 * 			method of assigning or reading vertex data. 
		 * 			
		 * @note	If vertex data of this type/semantic/index/stream doesn't exist and exception will be thrown.
		 */
		VertexElemIter<Vector2> getVec2DataIter(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

		/**
		 * @brief	Returns an iterator you can use for easily retrieving or setting Vector3 vertex elements. This is the preferred
		 * 			method of assigning or reading vertex data. 
		 * 			
		 * @note	If vertex data of this type/semantic/index/stream doesn't exist and exception will be thrown.
		 */
		VertexElemIter<Vector3> getVec3DataIter(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

		/**
		 * @brief	Returns an iterator you can use for easily retrieving or setting Vector4 vertex elements. This is the preferred
		 * 			method of assigning or reading vertex data. 
		 * 			
		 * @note	If vertex data of this type/semantic/index/stream doesn't exist and exception will be thrown.
		 */
		VertexElemIter<Vector4> getVec4DataIter(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

		/**
		 * @brief	Returns an iterator you can use for easily retrieving or setting DWORD vertex elements. This is the preferred
		 * 			method of assigning or reading vertex data. 
		 * 			
		 * @note	If vertex data of this type/semantic/index/stream doesn't exist and exception will be thrown.
		 */
		VertexElemIter<UINT32> getDWORDDataIter(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

		UINT32 getNumSubmeshes() const { return (UINT32)mSubMeshes.size(); }
		UINT32 getNumVertices() const { return mNumVertices; }
		UINT32 getNumIndices(UINT32 subMesh) const;
		UINT32 getNumIndices() const;
		DrawOperationType getDrawOp(UINT32 subMesh) const;
		DrawOperationType getDrawOp() const;

		UINT16* getIndices16(UINT32 subMesh = 0) const;
		UINT32* getIndices32(UINT32 subMesh = 0) const;
		UINT32 getIndexElementSize() const;
		IndexBuffer::IndexType getIndexType() const { return mIndexType; }

		/**
		 * @brief	Returns the pointer to the first element of the specified type. If you want to
		 * 			iterate over all elements you need to call getVertexStride() to get the number
		 * 			of bytes you need to advance between each element.
		 *
		 * @param	semantic   		Semantic that allows the engine to connect the data to a shader input slot.
		 * @param	semanticIdx 	(optional) If there are multiple semantics with the same name, use different index to differentiate between them.
		 * @param	streamIdx   	(optional) Zero-based index of the stream. Each stream will internally be represented as a single vertex buffer.
		 *
		 * @return	null if it fails, else the element data.
		 */
		UINT8* getElementData(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;
		UINT32 getElementOffset(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;

		const VertexDataDescPtr& getVertexDesc() const { return mVertexData; }

		static MeshDataPtr combine(const Vector<MeshDataPtr>::type& elements);

	protected:
		UINT32 getInternalBufferSize();

	private:
		friend class Mesh; // To avoid polluting the public interface with a bunch of nearly useless methods for outside world

		UINT32 mDescBuilding;

		UINT8* mData;

		UINT32 mNumVertices;
		UINT32 mNumIndices;
		DrawOperationType mDrawOp;
		IndexBuffer::IndexType mIndexType;

		Vector<IndexElementData>::type mSubMeshes;
		VertexDataDescPtr mVertexData;

		UINT8* getIndexData() const { return getData(); }
		UINT8* getStreamData(UINT32 streamIdx) const;

		UINT32 getIndexBufferOffset(UINT32 subMesh) const;
		UINT32 getStreamOffset(UINT32 streamIdx = 0) const;

		UINT32 getIndexBufferSize() const;
		UINT32 getStreamSize(UINT32 streamIdx) const;
		UINT32 getStreamSize() const;

		void getDataForIterator(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx, UINT8*& data, UINT32& stride) const;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	private:
		MeshData() {} // Serialization only

	public:
		friend class MeshDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}