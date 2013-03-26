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
					delete[] vertElem.data;
			}
		}

		for(auto& indexData : mIndices)
		{
			if(indexData.indices != nullptr)
				delete[] indexData.indices;
		}
	}

	void MeshData::setPositions(Vector2* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT2, VES_POSITION, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setPositions(Vector3* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT3, VES_POSITION, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setPositions(Vector4* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT4, VES_POSITION, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setNormals(Vector3* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT3, VES_NORMAL, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setTangents(Vector3* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT3, VES_TANGENT, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setTangents(Vector4* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT4, VES_TANGENT, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setBitangents(Vector3* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT3, VES_BITANGENT, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setUV0(Vector2* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT2, VES_TEXCOORD, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setUV1(Vector2* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_FLOAT2, VES_TEXCOORD, (UINT8*)(elements), numElements, 1, streamIdx);
	}

	void MeshData::setColors(Color* elements, UINT32 numElements, UINT32 streamIdx)
	{
		setVertexElementData(VET_COLOR, VES_COLOR, (UINT8*)(elements), numElements, 0, streamIdx);
	}

	void MeshData::setVertexElementData(VertexElementType type, VertexElementSemantic semantic, UINT8* elements, UINT32 numElements, UINT32 semanticIdx, UINT32 streamIdx)
	{
		clearIfItExists(type, semantic, semanticIdx, streamIdx);

		if(elements != nullptr)
		{
			vector<VertexElementData>::type& elemData = mVertexData[streamIdx];

			VertexElementData newElement(type, semantic, semanticIdx, streamIdx, elements, numElements);
			elemData.push_back(newElement);
		}
	}

	void MeshData::setIndices(UINT32* indices, UINT32 numIndices, UINT32 subMesh)
	{
		if(mIndexType != IndexBuffer::IT_32BIT)
			CM_EXCEPT(InvalidParametersException, "Trying to set 32bit indices but the MeshData was initialized as 16bit.");

		if(subMesh >= mIndices.size())
			mIndices.resize(subMesh + 1);

		IndexElementData indexData = mIndices[subMesh];

		if(indexData.indices != nullptr)
			delete[] indexData.indices;

		indexData.indices = (UINT8*)indices;
		indexData.numIndices = numIndices;
		indexData.elementSize = getIndexElementSize();
		indexData.subMesh = subMesh;

		mIndices[subMesh] = indexData;
	}

	void MeshData::setIndices(UINT16* indices, UINT32 numIndices, UINT32 subMesh)
	{
		if(mIndexType != IndexBuffer::IT_16BIT)
			CM_EXCEPT(InvalidParametersException, "Trying to set 16bit indices but the MeshData was initialized as 32bit.");

		if(subMesh >= mIndices.size())
			mIndices.resize(subMesh + 1);

		IndexElementData indexData = mIndices[subMesh];

		if(indexData.indices != nullptr)
			delete[] indexData.indices;

		indexData.indices = (UINT8*)indices;
		indexData.numIndices = numIndices;
		indexData.elementSize = getIndexElementSize();
		indexData.subMesh = subMesh;

		mIndices[subMesh] = indexData;
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
		MeshDataPtr combinedMeshData(new MeshData());

		UINT32 subMeshIndex = 0;
		vector<VertexElement>::type combinedVertexElements;
		vector<UINT8*>::type vertexElemData;
		UINT32 totalVertexCount = 0;

		UINT32 vertexIndexOffset = 0;
		for(auto& meshData : elements)
		{
			for(UINT32 i = 0; i < meshData->getNumSubmeshes(); i++)
			{
				UINT32 numIndices = meshData->getNumIndices(i);
				UINT32* indices = new UINT32[numIndices];

				UINT32* sourceIndices = meshData->getIndices32(i);

				for(UINT32 j = 0; j < numIndices; j++)
					indices[j] = sourceIndices[j] + vertexIndexOffset;

				combinedMeshData->setIndices(indices, numIndices, subMeshIndex);
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
					UINT32 newBufferSize = totalVertexCount * newElement.getSize();
					UINT8* newBuffer = new UINT8[newBufferSize];
					memset(newBuffer, 0, newBufferSize);

					vertexElemData.push_back(newBuffer);
					alreadyExistsIdx = (UINT32)vertexElemData.size() - 1;
				}

				UINT8* source = meshData->getVertElemData(newElement.getType(), newElement.getSemantic(), newElement.getIndex(), newElement.getSource()).data;
				memcpy(&(vertexElemData[alreadyExistsIdx]), source, numVertices * newElement.getSize());

				combinedMeshData->setVertexElementData(newElement.getType(), newElement.getSemantic(), source, numVertices, newElement.getIndex(), newElement.getSource());
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
				delete[] findIter->data;

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