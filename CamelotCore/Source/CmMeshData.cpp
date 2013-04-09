#include "CmMeshData.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmHardwareBufferManager.h"
#include "CmMeshDataRTTI.h"
#include "CmVertexDeclaration.h"
#include "CmException.h"

namespace CamelotEngine
{
	MeshData::MeshData(IndexBuffer::IndexType indexType)
	   :mIndexType(indexType)
	{

	}

	MeshData::~MeshData()
	{
		for(auto& vertElems : mVertexData)
		{
			for(auto& vertElem : vertElems.second)
			{
				if(vertElem.data != nullptr)
					CM_DELETE_BYTES(vertElem.data, ScratchAlloc);
			}
		}

		for(auto& indexData : mIndices)
		{
			if(indexData.indices != nullptr)
				CM_DELETE_BYTES(indexData.indices, ScratchAlloc);
		}
	}

	Vector2* MeshData::addPositionsVec2(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector2*>(addVertexElementData(VET_FLOAT2, VES_POSITION, numElements, 0, streamIdx));
	}

	Vector3* MeshData::addPositionsVec3(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector3*>(addVertexElementData(VET_FLOAT3, VES_POSITION, numElements, 0, streamIdx));
	}

	Vector4* MeshData::addPositionsVec4(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector4*>(addVertexElementData(VET_FLOAT4, VES_POSITION, numElements, 0, streamIdx));
	}

	Vector3* MeshData::addNormals(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector3*>(addVertexElementData(VET_FLOAT3, VES_NORMAL, numElements, 0, streamIdx));
	}

	Vector3* MeshData::addTangentsVec3(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector3*>(addVertexElementData(VET_FLOAT3, VES_TANGENT, numElements, 0, streamIdx));
	}

	Vector4* MeshData::addTangentsVec4(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector4*>(addVertexElementData(VET_FLOAT4, VES_TANGENT, numElements, 0, streamIdx));
	}

	Vector3* MeshData::addBitangents(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector3*>(addVertexElementData(VET_FLOAT3, VES_BITANGENT, numElements, 0, streamIdx));
	}

	Vector2* MeshData::addUV0(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector2*>(addVertexElementData(VET_FLOAT2, VES_TEXCOORD, numElements, 0, streamIdx));
	}

	Vector2* MeshData::addUV1(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Vector2*>(addVertexElementData(VET_FLOAT2, VES_TEXCOORD, numElements, 1, streamIdx));
	}

	Color* MeshData::addColorsFloat(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<Color*>(addVertexElementData(VET_FLOAT4, VES_COLOR, numElements, 0, streamIdx));
	}

	UINT32* MeshData::addColorsDWORD(UINT32 numElements, UINT32 streamIdx)
	{
		return reinterpret_cast<UINT32*>(addVertexElementData(VET_COLOR, VES_COLOR, numElements, 0, streamIdx));
	}

	UINT8* MeshData::addVertexElementData(VertexElementType type, VertexElementSemantic semantic, UINT32 numElements, UINT32 semanticIdx, UINT32 streamIdx)
	{
		clearIfItExists(type, semantic, semanticIdx, streamIdx);

		UINT32 elemSize = VertexElement::getTypeSize(type);
		UINT8* elements = CM_NEW_BYTES(elemSize * numElements, ScratchAlloc);

		vector<VertexElementData>::type& elemData = mVertexData[streamIdx];

		VertexElementData newElement(type, semantic, semanticIdx, streamIdx, elements, numElements);
		elemData.push_back(newElement);

		return elements;
	}

	UINT32* MeshData::addIndices32(UINT32 numIndices, UINT32 subMesh)
	{
		if(mIndexType != IndexBuffer::IT_32BIT)
			CM_EXCEPT(InvalidParametersException, "Trying to set 32bit indices but the MeshData was initialized as 16bit.");

		if(subMesh >= mIndices.size())
			mIndices.resize(subMesh + 1);

		IndexElementData indexData = mIndices[subMesh];

		if(indexData.indices != nullptr)
			CM_DELETE_BYTES(indexData.indices, ScratchAlloc);

		UINT32* indices = (UINT32*)CM_NEW_BYTES(numIndices * sizeof(UINT32), ScratchAlloc);

		indexData.indices = (UINT8*)indices;
		indexData.numIndices = numIndices;
		indexData.elementSize = getIndexElementSize();
		indexData.subMesh = subMesh;

		mIndices[subMesh] = indexData;

		return indices;
	}

	UINT16* MeshData::addIndices16(UINT32 numIndices, UINT32 subMesh)
	{
		if(mIndexType != IndexBuffer::IT_16BIT)
			CM_EXCEPT(InvalidParametersException, "Trying to set 16bit indices but the MeshData was initialized as 32bit.");

		if(subMesh >= mIndices.size())
			mIndices.resize(subMesh + 1);

		IndexElementData indexData = mIndices[subMesh];

		if(indexData.indices != nullptr)
			CM_DELETE_BYTES(indexData.indices, ScratchAlloc);

		UINT16* indices = (UINT16*)CM_NEW_BYTES(numIndices * sizeof(UINT16), ScratchAlloc);

		indexData.indices = (UINT8*)indices;
		indexData.numIndices = numIndices;
		indexData.elementSize = getIndexElementSize();
		indexData.subMesh = subMesh;

		mIndices[subMesh] = indexData;

		return indices;
	}

	VertexDeclarationPtr MeshData::createDeclaration() const
	{
		VertexDeclarationPtr declaration = HardwareBufferManager::instance().createVertexDeclaration();

		for(auto& vertElems : mVertexData)
		{
			UINT32 offset = 0;
			for(auto& vertElem : vertElems.second)
			{
				declaration->addElement(vertElems.first, offset, vertElem.element.getType(), vertElem.element.getSemantic(), vertElem.element.getIndex());
				offset += vertElem.element.getSize();
			}
		}

		return declaration;
	}

	UINT32 MeshData::getNumVertices() const
	{
		UINT32 numVertices = 0;

		auto vertElemDataPerStream = mVertexData;
		MeshData::VertexElementData* firstElemData = nullptr;
		if(vertElemDataPerStream.size() > 0)
		{
			auto vertElemData = vertElemDataPerStream.begin()->second;
			auto firstVertElem = vertElemData.begin();
			if(firstVertElem != vertElemData.end())
			{
				numVertices = firstVertElem->elementCount;
			}
		}

		for(auto& vertElems : mVertexData)
		{
			for(auto& vertElem : vertElems.second)
			{
				if(vertElem.elementCount != numVertices)
				{
					CM_EXCEPT(InvalidParametersException, "All vertex element arrays in MeshData need to be of the same size. Found an array with semantic: " \
						+ toString(vertElem.element.getSemantic()) + " and element count: " + toString(vertElem.elementCount) + ". This doesn't match with other " \
						+ "element with semantic: " + toString(firstElemData->element.getSemantic()) + " and element count: " + toString(firstElemData->elementCount));
				}
			}
		}

		return numVertices;
	}

	UINT32 MeshData::getNumIndices(UINT32 subMesh) const
	{
		return mIndices.at(subMesh).numIndices;
	}

	UINT16* MeshData::getIndices16(UINT32 subMesh) const
	{
		return (UINT16*)mIndices.at(subMesh).indices;
	}

	UINT32* MeshData::getIndices32(UINT32 subMesh) const
	{
		return (UINT32*)mIndices.at(subMesh).indices;
	}

	vector<VertexElement>::type MeshData::getVertexElements() const
	{
		vector<VertexElement>::type elements;
		for(auto& vertElems : mVertexData)
		{
			UINT32 offset = 0;
			for(auto& vertElem : vertElems.second)
			{
				elements.push_back(vertElem.element);
			}
		}

		return elements;
	}

	MeshDataPtr MeshData::combine(const vector<MeshDataPtr>::type& elements)
	{
		MeshDataPtr combinedMeshData(CM_NEW(MeshData, PoolAlloc) MeshData(),
			&MemAllocDeleter<MeshData, PoolAlloc>::deleter);

		UINT32 subMeshIndex = 0;
		vector<VertexElement>::type combinedVertexElements;
		vector<UINT8*>::type vertexElemData;
		vector<UINT32>::type bufferOffsets;
		UINT32 totalVertexCount = 0;

		UINT32 vertexIndexOffset = 0;
		for(auto& meshData : elements)
		{
			for(UINT32 i = 0; i < meshData->getNumSubmeshes(); i++)
			{
				UINT32 numIndices = meshData->getNumIndices(i);
				UINT32* indices = combinedMeshData->addIndices32(numIndices, subMeshIndex);

				UINT32* sourceIndices = meshData->getIndices32(i);

				for(UINT32 j = 0; j < numIndices; j++)
					indices[j] = sourceIndices[j] + vertexIndexOffset;

				subMeshIndex++;
			}

			UINT32 numVertices = meshData->getNumVertices();
			totalVertexCount += numVertices;
			vertexIndexOffset += numVertices;
		}

		UINT32 vertexOffset = 0;
		for(auto& meshData : elements)
		{
			vector<VertexElement>::type vertexElements = meshData->getVertexElements();
			UINT32 numVertices = meshData->getNumVertices();

			for(auto& newElement : vertexElements)
			{
				INT32 alreadyExistsIdx = -1;
				UINT32 idx = 0;
				for(auto& existingElement : combinedVertexElements)
				{
					if(newElement == existingElement)
					{
						alreadyExistsIdx = idx;
						break;
					}

					idx++;
				}

				if(alreadyExistsIdx == -1)
				{
					combinedVertexElements.push_back(newElement);
					
					UINT8* newBuffer = combinedMeshData->addVertexElementData(newElement.getType(), newElement.getSemantic(), totalVertexCount, newElement.getIndex(), newElement.getSource());
					
					UINT32 newBufferSize = totalVertexCount * newElement.getSize();
					memset(newBuffer, 0, newBufferSize);

					vertexElemData.push_back(newBuffer);
					bufferOffsets.push_back(0);
					alreadyExistsIdx = (UINT32)vertexElemData.size() - 1;
				}

				UINT8* source = meshData->getVertElemData(newElement.getType(), newElement.getSemantic(), newElement.getIndex(), newElement.getSource()).data;
				UINT32 offset = vertexOffset * newElement.getSize();

				memcpy(&(vertexElemData[alreadyExistsIdx]) + offset, source, numVertices * newElement.getSize());
			}

			vertexOffset += meshData->getNumVertices();
		}

		return combinedMeshData;
	}

	bool MeshData::hasElement(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
	{
		auto elemDataIter = mVertexData.find(streamIdx);

		if(elemDataIter == mVertexData.end())
			return false;

		const vector<VertexElementData>::type& elemData = elemDataIter->second;

		auto findIter = std::find_if(elemData.begin(), elemData.end(), 
			[semantic, semanticIdx] (const VertexElementData& x) 
		{ 
			return x.element.getSemantic() == semantic && x.element.getIndex() == semanticIdx; 
		});

		if(findIter != elemData.end())
			return true;

		return false;
	}

	void MeshData::clearIfItExists(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		vector<VertexElementData>::type& elemData = mVertexData[streamIdx];

		auto findIter = std::find_if(elemData.begin(), elemData.end(), 
			[type, semantic, semanticIdx] (const VertexElementData& x) 
		{ 
			return x.element.getSemantic() == semantic && x.element.getIndex() == semanticIdx; 
		});

		if(findIter != elemData.end())
		{
			if(findIter->data != nullptr)
				CM_DELETE_BYTES(findIter->data, ScratchAlloc);

			elemData.erase(findIter);
		}
	}

	MeshData::VertexElementData& MeshData::getVertElemData(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		vector<VertexElementData>::type& elemData = mVertexData[streamIdx];

		auto findIter = std::find_if(elemData.begin(), elemData.end(), 
			[type, semantic, semanticIdx] (const VertexElementData& x) 
		{ 
			return x.element.getSemantic() == semantic && x.element.getIndex() == semanticIdx; 
		});

		if(findIter == elemData.end())
			CM_EXCEPT(InvalidParametersException, "No vertex element of specified type exists.");

		return *findIter;
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* MeshData::VertexElementData::getRTTIStatic()
	{
		return VertexElementDataRTTI::instance();
	}

	RTTITypeBase* MeshData::VertexElementData::getRTTI() const
	{
		return VertexElementData::getRTTIStatic();
	}

	RTTITypeBase* MeshData::IndexElementData::getRTTIStatic()
	{
		return IndexElementDataRTTI::instance();
	}

	RTTITypeBase* MeshData::IndexElementData::getRTTI() const
	{
		return IndexElementData::getRTTIStatic();
	}

	RTTITypeBase* MeshData::getRTTIStatic()
	{
		return MeshDataRTTI::instance();
	}

	RTTITypeBase* MeshData::getRTTI() const
	{
		return MeshData::getRTTIStatic();
	}
}