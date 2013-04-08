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
		 * @brief	Allocates a buffer for holding a specified amount of vertex positions, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector2* addPositionsVec2(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex positions, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector3* addPositionsVec3(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex positions, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector4* addPositionsVec4(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex normals, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector3* addNormals(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex tangents, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector3* addTangentsVec3(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex tangents, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector4* addTangentsVec4(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex bitangents, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector3* addBitangents(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex texture coordinates, 
		 * 			and returns a pointer the user can use to populate the buffer. If a previous set 
		 * 			of data exists in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector2* addUV0(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex texture coordinates, 
		 * 			and returns a pointer the user can use to populate the buffer. If a previous set 
		 * 			of data exists in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Vector2* addUV1(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex colors, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		Color* addColorsFloat(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		 * @brief	Allocates a buffer for holding a specified amount of vertex colors, and returns a
		 * 			pointer the user can use to populate the buffer. If a previous set of data exists 
		 * 			in this same stream slot, it will be deleted.
		 *
		 * @param	numElements			Number of elements in the elements array.
		 * @param	streamIdx			(optional) Zero-based index of the stream. Each stream will 
		 * 								internally be represented as a single vertex buffer.
		 */
		UINT32* addColorsDWORD(UINT32 numElements, UINT32 streamIdx = 0);

		/**
		* @brief	Allocates a buffer for holding a specified amount of vertex data, and returns a
		* 			pointer the user can use to populate the buffer. Anything that was previously
		 * 			present at the same data slot is removed.
		 *
		 * @param	type	   	Type of the vertex element. Determines size.
		 * @param	semantic   	Semantic that allows the engine to connect the data to a shader input slot.
		 * @param	numElements	Number of elements in the array.
		 * @param	semanticIdx	(optional) If there are multiple semantics with the same name, use different index to differentiate between them.
		 * @param	streamIdx  	(optional) Zero-based index of the stream. Each stream will internally be represented as a single vertex buffer.
		 */
		UINT8* addVertexElementData(VertexElementType type, VertexElementSemantic semantic, UINT32 numElements, UINT32 semanticIdx = 0, UINT32 streamIdx = 0);

		/**
		* @brief	Allocates buffer for the indices for the specified sub mesh, and returns a
		* 			pointer the user can use to populate the buffer. Any indexes previously
		 * 			set for the sub mesh are deleted.
		 *
		 * @param	numIndices	   	Number of indices.
		 * @param	subMesh		   	(optional) the sub mesh.
		 */
		UINT32* addIndices32(UINT32 numIndices, UINT32 subMesh = 0);

		/**
		* @brief	Allocates buffer for the indices for the specified sub mesh, and returns a
		* 			pointer the user can use to populate the buffer. Any indexes previously
		 * 			set for the sub mesh are deleted.
		 *
		 * @param	numIndices	   	Number of indices.
		 * @param	subMesh		   	(optional) the sub mesh.
		 */
		UINT16* addIndices16(UINT32 numIndices, UINT32 subMesh = 0);

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