#include "CmMeshData.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmHardwareBufferManager.h"
#include "CmMeshDataRTTI.h"
#include "CmVertexDeclaration.h"
#include "CmException.h"

namespace CamelotFramework
{
	MeshData::MeshData(UINT32 numVertices, IndexBuffer::IndexType indexType)
	   :mNumVertices(numVertices), mIndexType(indexType), mData(nullptr), mDescBuilding(false)
	{

	}

	MeshData::~MeshData()
	{

	}

	void MeshData::beginDesc()
	{
		if(mDescBuilding)
			CM_EXCEPT(InternalErrorException, "beginDesc() but description building has already began.");

		mVertexElements.clear();
		mSubMeshes.clear();

		mDescBuilding = true;
	}

	void MeshData::endDesc()
	{
		if(!mDescBuilding)
			CM_EXCEPT(InternalErrorException, "endDesc() called without beginDesc().");

		allocateInternalBuffer();

		mDescBuilding = false;
	}

	void MeshData::addVertElem(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		if(!mDescBuilding)
			CM_EXCEPT(InternalErrorException, "Cannot add vertex element when not building description. Call beginDesc() first.");

		clearIfItExists(type, semantic, semanticIdx, streamIdx);

		VertexElement newElement(streamIdx, 0, type, semantic, semanticIdx);

		// Insert it so it is sorted by stream
		UINT32 insertToIndex = (UINT32)mVertexElements.size();
		UINT32 idx = 0;
		for(auto& elem : mVertexElements)
		{
			if(elem.getStreamIdx() > streamIdx)
			{
				insertToIndex = idx;
				break;
			}

			idx++;
		}

		mVertexElements.insert(mVertexElements.begin() + insertToIndex, newElement);
	}

	void MeshData::addSubMesh(UINT32 numIndices, UINT32 subMesh, DrawOperationType drawOp)
	{
		if(!mDescBuilding)
			CM_EXCEPT(InternalErrorException, "Cannot add indices when not building description. Call beginDesc() first.");

		if(subMesh >= mSubMeshes.size())
			mSubMeshes.resize(subMesh + 1);

		IndexElementData indexData = mSubMeshes[subMesh];

		indexData.numIndices = numIndices;
		indexData.elementSize = getIndexElementSize();
		indexData.subMesh = subMesh;
		indexData.drawOp = drawOp;

		mSubMeshes[subMesh] = indexData;
	}

	VertexDeclarationPtr MeshData::createDeclaration() const
	{
		VertexDeclarationPtr declaration = HardwareBufferManager::instance().createVertexDeclaration();

		UINT32 maxStreamIdx = getMaxStreamIdx();

		UINT32 numStreams = maxStreamIdx + 1;
		UINT32* streamOffsets = cm_newN<UINT32, ScratchAlloc>(numStreams);
		for(UINT32 i = 0; i < numStreams; i++)
			streamOffsets[i] = 0;

		for(auto& vertElem : mVertexElements)
		{
			UINT32 streamIdx = vertElem.getStreamIdx();
			declaration->addElement(streamIdx, streamOffsets[streamIdx], vertElem.getType(), vertElem.getSemantic(), vertElem.getSemanticIdx());
			streamOffsets[streamIdx] += vertElem.getSize();
		}

		cm_deleteN<ScratchAlloc>(streamOffsets, numStreams);

		return declaration;
	}

	UINT32 MeshData::getNumIndices(UINT32 subMesh) const
	{
		return mSubMeshes.at(subMesh).numIndices;
	}

	DrawOperationType MeshData::getDrawOp(UINT32 subMesh) const
	{
		return mSubMeshes.at(subMesh).drawOp;
	}

	UINT32 MeshData::getNumIndices() const
	{
		UINT32 count = 0;
		for(UINT32 i = 0; i < getNumSubmeshes(); i++)
		{
			count += mSubMeshes[i].numIndices;
		}

		return count;
	}

	UINT16* MeshData::getIndices16(UINT32 subMesh) const
	{
		if(mIndexType != IndexBuffer::IT_16BIT)
			CM_EXCEPT(InternalErrorException, "Attempting to get 16bit index buffer, but internally allocated buffer is 32 bit.");

		UINT32 indexBufferOffset = getIndexBufferOffset(subMesh);

		return (UINT16*)(getData() + indexBufferOffset);
	}

	UINT32* MeshData::getIndices32(UINT32 subMesh) const
	{
		if(mIndexType != IndexBuffer::IT_32BIT)
			CM_EXCEPT(InternalErrorException, "Attempting to get 32bit index buffer, but internally allocated buffer is 16 bit.");

		UINT32 indexBufferOffset = getIndexBufferOffset(subMesh);

		return (UINT32*)(getData() + indexBufferOffset);
	}

	UINT32 MeshData::getMaxStreamIdx() const
	{
		UINT32 maxStreamIdx = 0;
		for(auto& vertElems : mVertexElements)
		{
			UINT32 offset = 0;
			for(auto& vertElem : mVertexElements)
			{
				maxStreamIdx = std::max((UINT32)maxStreamIdx, (UINT32)vertElem.getStreamIdx());
			}
		}

		return maxStreamIdx;
	}

	bool MeshData::hasStream(UINT32 streamIdx) const
	{
		for(auto& vertElem : mVertexElements)
		{
			if(vertElem.getStreamIdx() == streamIdx)
				return true;
		}

		return false;
	}

	UINT32 MeshData::getInternalBufferSize()
	{
		return getIndexBufferSize() + getStreamSize();
	}

	// TODO - This doesn't handle the case where multiple elements in same slot have different data types
	//  - actually it will likely corrupt memory in that case
	MeshDataPtr MeshData::combine(const Vector<MeshDataPtr>::type& meshes)
	{
		UINT32 totalVertexCount = 0;
		for(auto& meshData : meshes)
		{
			UINT32 numVertices = meshData->getNumVertices();
			totalVertexCount += numVertices;
		}

		MeshDataPtr combinedMeshData = cm_shared_ptr<MeshData, PoolAlloc>(totalVertexCount);

		combinedMeshData->beginDesc();

		UINT32 subMeshIndex = 0;
		for(auto& meshData : meshes)
		{
			for(UINT32 i = 0; i < meshData->getNumSubmeshes(); i++)
			{
				UINT32 numIndices = meshData->getNumIndices(i);
				combinedMeshData->addSubMesh(numIndices, subMeshIndex);

				subMeshIndex++;
			}
		}

		Vector<VertexElement>::type combinedVertexElements;
		for(auto& meshData : meshes)
		{
			for(auto& newElement : meshData->mVertexElements)
			{
				INT32 alreadyExistsIdx = -1;
				UINT32 idx = 0;
				for(auto& existingElement : combinedVertexElements)
				{
					if(newElement.getSemantic() == existingElement.getSemantic() && newElement.getSemanticIdx() == existingElement.getSemanticIdx()
						&& newElement.getStreamIdx() == existingElement.getStreamIdx())
					{
						if(newElement.getType() != existingElement.getType())
						{
							CM_EXCEPT(NotImplementedException, "Two elements have same semantics but different types. This is not supported yet.");
						}

						alreadyExistsIdx = idx;
						break;
					}

					idx++;
				}

				if(alreadyExistsIdx == -1)
				{
					combinedVertexElements.push_back(newElement);
					combinedMeshData->addVertElem(newElement.getType(), newElement.getSemantic(), newElement.getSemanticIdx(), newElement.getStreamIdx());
				}
			}
		}

		combinedMeshData->endDesc();

		// Copy indices
		subMeshIndex = 0;
		UINT32 vertexOffset = 0;
		for(auto& meshData : meshes)
		{
			for(UINT32 i = 0; i < meshData->getNumSubmeshes(); i++)
			{
				UINT32 numIndices = meshData->getNumIndices(i);
				UINT32* srcData = meshData->getIndices32(i);

				UINT32* dstData = combinedMeshData->getIndices32(subMeshIndex);

				for(UINT32 j = 0; j < numIndices; j++)
					dstData[j] = srcData[j] + vertexOffset;

				subMeshIndex++;
			}

			vertexOffset += meshData->getNumVertices();
		}

		// Copy vertices
		vertexOffset = 0;
		for(auto& meshData : meshes)
		{
			for(auto& element : combinedMeshData->mVertexElements)
			{
				UINT32 dstVertexStride = combinedMeshData->getVertexStride(element.getStreamIdx());
				UINT8* dstData = combinedMeshData->getElementData(element.getSemantic(), element.getSemanticIdx(), element.getStreamIdx());
				dstData += vertexOffset * dstVertexStride;

				UINT32 numSrcVertices = meshData->getNumVertices();
				UINT32 vertexSize = combinedMeshData->getElementSize(element.getSemantic(), element.getSemanticIdx(), element.getStreamIdx());

				if(meshData->hasElement(element.getSemantic(), element.getSemanticIdx(), element.getStreamIdx()))
				{
					UINT32 srcVertexStride = meshData->getVertexStride(element.getStreamIdx());
					UINT8* srcData = meshData->getElementData(element.getSemantic(), element.getSemanticIdx(), element.getStreamIdx());

					for(UINT32 i = 0; i < numSrcVertices; i++)
					{
						memcpy(dstData, srcData, vertexSize);
						dstData += dstVertexStride;
						srcData += srcVertexStride;
					}
				}
				else
				{
					for(UINT32 i = 0; i < numSrcVertices; i++)
					{
						memset(dstData, 0, vertexSize);
						dstData += dstVertexStride;
					}
				}
			}

			vertexOffset += meshData->getNumVertices();
		}

		return combinedMeshData;
	}

	bool MeshData::hasElement(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
	{
		auto findIter = std::find_if(mVertexElements.begin(), mVertexElements.end(), 
			[semantic, semanticIdx, streamIdx] (const VertexElement& x) 
		{ 
			return x.getSemantic() == semantic && x.getSemanticIdx() == semanticIdx && x.getStreamIdx() == streamIdx; 
		});

		if(findIter != mVertexElements.end())
		{
			return true;
		}

		return false;
	}

	void MeshData::setVertexData(VertexElementSemantic semantic, UINT8* data, UINT32 size, UINT32 semanticIdx, UINT32 streamIdx)
	{
		assert(data != nullptr);

		if(!hasElement(semantic, semanticIdx, streamIdx))
		{
			CM_EXCEPT(InvalidParametersException, "MeshData doesn't contain an element of specified type: Semantic: " + toString(semantic) + ", Semantic index: "
				+ toString(semanticIdx) + ", Stream index: " + toString(streamIdx));
		}

		UINT32 elementSize = getElementSize(semantic, semanticIdx, streamIdx);
		UINT32 totalSize = elementSize * mNumVertices;

		if(totalSize != size)
		{
			CM_EXCEPT(InvalidParametersException, "Buffer sizes don't match. Expected: " + toString(totalSize) + ". Got: " + toString(size));
		}

		UINT32 indexBufferOffset = getIndexBufferSize();

		UINT32 elementOffset = getElementOffset(semantic, semanticIdx, streamIdx);
		UINT32 vertexStride = getVertexStride(streamIdx);

		UINT8* dst = getData() + indexBufferOffset + elementOffset;
		UINT8* src = data;
		for(UINT32 i = 0; i < mNumVertices; i++)
		{
			memcpy(dst, src, elementSize);
			dst += vertexStride;
			src += elementSize;
		}
	}

	VertexElemIter<Vector2> MeshData::getVec2DataIter(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		UINT8* data;
		UINT32 vertexStride;
		getDataForIterator(semantic, semanticIdx, streamIdx, data, vertexStride);

		return VertexElemIter<Vector2>(data, vertexStride, mNumVertices);
	}

	VertexElemIter<Vector3> MeshData::getVec3DataIter(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		UINT8* data;
		UINT32 vertexStride;
		getDataForIterator(semantic, semanticIdx, streamIdx, data, vertexStride);

		return VertexElemIter<Vector3>(data, vertexStride, mNumVertices);
	}

	VertexElemIter<Vector4> MeshData::getVec4DataIter(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		UINT8* data;
		UINT32 vertexStride;
		getDataForIterator(semantic, semanticIdx, streamIdx, data, vertexStride);

		return VertexElemIter<Vector4>(data, vertexStride, mNumVertices);
	}

	VertexElemIter<Color> MeshData::getColorDataIter(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		UINT8* data;
		UINT32 vertexStride;
		getDataForIterator(semantic, semanticIdx, streamIdx, data, vertexStride);

		return VertexElemIter<Color>(data, vertexStride, mNumVertices);
	}

	VertexElemIter<UINT32> MeshData::getDWORDDataIter(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		UINT8* data;
		UINT32 vertexStride;
		getDataForIterator(semantic, semanticIdx, streamIdx, data, vertexStride);

		return VertexElemIter<UINT32>(data, vertexStride, mNumVertices);
	}

	void MeshData::getDataForIterator(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx, UINT8*& data, UINT32& stride) const
	{
		if(!hasElement(semantic, semanticIdx, streamIdx))
		{
			CM_EXCEPT(InvalidParametersException, "MeshData doesn't contain an element of specified type: Semantic: " + toString(semantic) + ", Semantic index: "
				+ toString(semanticIdx) + ", Stream index: " + toString(streamIdx));
		}

		UINT32 indexBufferOffset = getIndexBufferSize();

		UINT32 elementOffset = getElementOffset(semantic, semanticIdx, streamIdx);

		data = getData() + indexBufferOffset + elementOffset;
		stride = getVertexStride(streamIdx);
	}

	UINT32 MeshData::getIndexBufferOffset(UINT32 subMesh) const
	{
		if(subMesh < 0 || (subMesh > (UINT32)mSubMeshes.size()))
		{
			CM_EXCEPT(InvalidParametersException, "Submesh out of range: " + toString(subMesh) + ". Allowed range: 0 .. " + toString((UINT32)mSubMeshes.size()));
		}

		UINT32 offset = 0;
		for(UINT32 i = 0; i < subMesh; i++)
		{
			offset += mSubMeshes[i].numIndices * getIndexElementSize();
		}

		return offset;
	}

	UINT32 MeshData::getStreamOffset(UINT32 streamIdx) const
	{
		UINT32 streamOffset = 0;
		bool found = false;
		for(auto& element : mVertexElements)
		{
			if(element.getStreamIdx() == streamIdx)
			{
				found = true;
				break;
			}

			streamOffset += element.getSize();
		}

		if(!found)
			CM_EXCEPT(InternalErrorException, "Cannot find the specified stream: " + toString(streamIdx));

		return streamOffset * mNumVertices;
	}

	UINT32 MeshData::getElementSize(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
	{
		for(auto& element : mVertexElements)
		{
			if(element.getSemantic() == semantic && element.getSemanticIdx() == semanticIdx && element.getStreamIdx() == streamIdx)
				return element.getSize();
		}

		return -1;
	}

	UINT8* MeshData::getElementData(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
	{ 
		return getData() + getIndexBufferSize() + getElementOffset(semantic, semanticIdx, streamIdx); 
	}

	UINT8* MeshData::getStreamData(UINT32 streamIdx) const
	{ 
		return getData() + getIndexBufferSize() + getStreamOffset(streamIdx); 
	}

	UINT32 MeshData::getIndexElementSize() const
	{
		return mIndexType == IndexBuffer::IT_32BIT ? sizeof(UINT32) : sizeof(UINT16);
	}

	UINT32 MeshData::getElementOffset(VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx) const
	{
		UINT32 streamOffset = getStreamOffset(streamIdx);

		UINT32 vertexOffset = 0;
		for(auto& element : mVertexElements)
		{
			if(element.getStreamIdx() != streamIdx)
				continue;

			if(element.getSemantic() == semantic && element.getSemanticIdx() == semanticIdx)
				break;

			vertexOffset += element.getSize();
		}

		return streamOffset * mNumVertices + vertexOffset;
	}

	UINT32 MeshData::getStreamSize(UINT32 streamIdx) const
	{
		UINT32 vertexStride = 0;
		for(auto& element : mVertexElements)
		{
			if(element.getStreamIdx() == streamIdx)
				vertexStride += element.getSize();
		}

		return vertexStride * mNumVertices;
	}

	UINT32 MeshData::getStreamSize() const
	{
		UINT32 vertexStride = 0;
		for(auto& element : mVertexElements)
		{
			vertexStride += element.getSize();
		}

		return vertexStride * mNumVertices;
	}

	UINT32 MeshData::getVertexStride(UINT32 streamIdx) const
	{
		UINT32 vertexStride = 0;
		for(auto& element : mVertexElements)
		{
			if(element.getStreamIdx() == streamIdx)
				vertexStride += element.getSize();
		}

		return vertexStride;
	}

	void MeshData::clearIfItExists(VertexElementType type, VertexElementSemantic semantic, UINT32 semanticIdx, UINT32 streamIdx)
	{
		auto findIter = std::find_if(mVertexElements.begin(), mVertexElements.end(), 
			[semantic, semanticIdx, streamIdx] (const VertexElement& x) 
		{ 
			return x.getSemantic() == semantic && x.getSemanticIdx() == semanticIdx && x.getStreamIdx() == streamIdx; 
		});

		if(findIter != mVertexElements.end())
		{
			mVertexElements.erase(findIter);
		}
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* MeshData::getRTTIStatic()
	{
		return MeshDataRTTI::instance();
	}

	RTTITypeBase* MeshData::getRTTI() const
	{
		return MeshData::getRTTIStatic();
	}
}