#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmVertexBuffer.h"
#include "CmIndexBuffer.h"
#include "CmVertexDeclaration.h"

namespace CamelotEngine
{
	class CM_EXPORT MeshData : public IReflectable
	{
	public:
		struct VertexElementData : public IReflectable
		{
			VertexElementData(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx, UINT8* _data, UINT32 numElements)
				:data(_data), elementCount(numElements), element(streamIdx, 0, type, semantic, semanticIdx)
			{ }

			UINT8* data;
			UINT32 elementCount;
			VertexElement element;

			/************************************************************************/
			/* 								SERIALIZATION                      		*/
			/************************************************************************/
		public:
			VertexElementData()
			:data(nullptr), elementCount(0) 
			{} // Serialization only constructor

			friend class VertexElementDataRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		struct IndexElementData : public IReflectable
		{
			IndexElementData()
				:numIndices(0), subMesh(0), elementSize(0), indices(nullptr)
			{ }

			UINT8* indices;
			UINT32 numIndices;
			UINT32 elementSize;
			UINT32 subMesh;

			/************************************************************************/
			/* 								SERIALIZATION                      		*/
			/************************************************************************/
		public:
			friend class IndexElementDataRTTI;
			static RTTITypeBase* getRTTIStatic();
			virtual RTTITypeBase* getRTTI() const;
		};

		MeshData(IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT);
		~MeshData();

		/**
		 * @brief	Provides a convenient way of setting mesh positions. 
		 *
		 * @param 	elements			Pointer to pre-allocated array of positions. Any previous position data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setPositions(Vector2* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh positions. 
		 *
		 * @param 	elements			Pointer to pre-allocated array of positions. Any previous position data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setPositions(Vector3* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh positions. 
		 *
		 * @param 	elements			Pointer to pre-allocated array of positions. Any previous position data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 * 		 
		 */
		void setPositions(Vector4* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh normals.
		 *
		 * @param 	elements			Pointer to pre-allocated array of normals. Any previous normal data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setNormals(Vector3* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh tangents.
		 *
		 * @param 	elements			Pointer to pre-allocated array of tangents. Any previous tangent data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setTangents(Vector3* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh tangents.
		 *
		 * @param 	elements			Pointer to pre-allocated array of tangents. Any previous tangent data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setTangents(Vector4* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh bitangents. 
		 *
		 * @param 	elements			Pointer to pre-allocated array of bitangents. Any previous bitangent data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setBitangents(Vector3* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh texture coordinates.
		 *
		 * @param 	elements			Pointer to pre-allocated array of texture coordinates. Any previous uv0 data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setUV0(Vector2* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh texture coordinates. 
		 *
		 * @param 	elements			Pointer to pre-allocated array of texture coordinates. Any previous uv1 data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setUV1(Vector2* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Provides a convenient way of setting mesh colors. 
		 *
		 * @param 	elements			Pointer to pre-allocated array of colors. Any previous color data will be deleted.
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 * 								
		 * @note MeshData will take ownership of the provided memory, and will delete it upon 
		 * 		 destruction or when you replace it with other data.
		 */
		void setColors(Color* elements, UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Adds (or replaces) a new set of vertex element data. Anything that was previously
		 * 			present at the same data slot is removed.
		 *
		 * @param	type	   	Type of the vertex element. Determines size.
		 * @param	semantic   	Semantic that allows the engine to connect the data to a shader input slot.
		 * @param	elements   	Allocated array of elements. Total size should be size of element type * number of elements. Any previous data at the same slot will be deleted.
		 * @param	numElements	Number of elements in the array.
		 * @param	semanticIdx	(optional) If there are multiple semantics with the same name, use different index to differentiate between them.
		 * @param	streamIdx  	(optional) Zero-based index of the stream. Each stream will internally be represented as a single vertex buffer.
		 */
		void setVertexElementData(VertexElementType type, VertexElementSemantic semantic, UINT8* elements, UINT32 numElements, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

		/**
		 * @brief	Sets a list of indices for the specified sub mesh. Any indexes previously
		 * 			set for the sub mesh are deleted.
		 *
		 * @param 	indices	If non-null, the indices.
		 * @param	numIndices	   	Number of indices.
		 * @param	subMesh		   	(optional) the sub mesh.
		 */
		void setIndices(UINT32* indices, UINT32 numIndices, UINT32 subMesh = 0);

		/**
		 * @brief	Sets a list of indices for the specified sub mesh. Any indexes previously
		 * 			set for the sub mesh are deleted.
		 *
		 * @param 	indices	If non-null, the indices.
		 * @param	numIndices	   	Number of indices.
		 * @param	subMesh		   	(optional) the sub mesh.
		 */
		void setIndices(UINT16* indices, UINT32 numIndices, UINT32 subMesh = 0);

		/**
		 * @brief	Query if we have vertex data for the specified semantic.
		 */
		bool hasElement(VertexElementSemantic semantic, UINT32 semanticIdx = 0, UINT32 streamIdx = 0) const;

		/**
		 * @brief	Creates a new vertex declaration based on set vertex elements.
		 */
		VertexDeclarationPtr createDeclaration() const;

		UINT32 getNumSubmeshes() const { return (UINT32)mIndices.size(); }
		UINT32 getNumVertices() const;
		UINT32 getNumIndices(UINT32 subMesh) const;

		UINT16* getIndices16(UINT32 subMesh) const;
		UINT32* getIndices32(UINT32 subMesh) const;

		vector<VertexElement>::type getVertexElements() const;

		MeshData::VertexElementData& getVertElemData(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx);

		UINT32 getIndexElementSize()
		{
			return mIndexType == IndexBuffer::IT_32BIT ? sizeof(UINT32) : sizeof(UINT16);
		}

		static MeshDataPtr combine(const vector<MeshDataPtr>::type& elements);

	private:
		friend class Mesh;

		vector<IndexElementData>::type mIndices;
		map<UINT32, vector<VertexElementData>::type>::type mVertexData;

		IndexBuffer::IndexType mIndexType;

		void clearIfItExists(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx);

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshDataRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}