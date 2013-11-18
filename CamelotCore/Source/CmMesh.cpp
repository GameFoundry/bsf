#include "CmMesh.h"
#include "CmMeshRTTI.h"
#include "CmMeshData.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmDebug.h"
#include "CmHardwareBufferManager.h"
#include "CmMeshManager.h"
#include "CmCoreThread.h"
#include "CmAsyncOp.h"
#include "CmAABox.h"
#include "CmVertexDataDesc.h"

#include "CmProfiler.h"

namespace CamelotFramework
{
	Mesh::Mesh()
		:mVertexData(nullptr), mIndexData(nullptr)
	{

	}

	Mesh::~Mesh()
	{
	}

	void Mesh::writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(data.getTypeId() != TID_MeshData)
			CM_EXCEPT(InvalidParametersException, "Invalid GpuResourceData type. Only MeshData is supported.");

		const MeshData& meshData = static_cast<const MeshData&>(data);

		// Indices
		mIndexData = std::shared_ptr<IndexData>(cm_new<IndexData, PoolAlloc>());

		mIndexData->indexCount = meshData.getNumIndices();
		mIndexData->indexBuffer = HardwareBufferManager::instance().createIndexBuffer(
			meshData.getIndexType(),
			mIndexData->indexCount, 
			GBU_STATIC);

		UINT8* idxData = static_cast<UINT8*>(mIndexData->indexBuffer->lock(GBL_WRITE_ONLY));
		UINT32 idxElementSize = meshData.getIndexElementSize();

		UINT32 indicesSize = meshData.getIndexBufferSize();
		UINT8* srcIdxData = meshData.getIndexData(); 

		memcpy(idxData, srcIdxData, indicesSize);

		mIndexData->indexBuffer->unlock();

		// Vertices
		mVertexData = std::shared_ptr<VertexData>(cm_new<VertexData, PoolAlloc>());

		mVertexData->vertexCount = meshData.getNumVertices();
		mVertexData->vertexDeclaration = meshData.getVertexDesc()->createDeclaration();

		for(UINT32 i = 0; i <= meshData.getVertexDesc()->getMaxStreamIdx(); i++)
		{
			if(!meshData.getVertexDesc()->hasStream(i))
				continue;

			UINT32 streamSize = meshData.getStreamSize(i);

			VertexBufferPtr vertexBuffer = HardwareBufferManager::instance().createVertexBuffer(
				mVertexData->vertexDeclaration->getVertexSize(i),
				mVertexData->vertexCount,
				GBU_STATIC);


			mVertexData->setBuffer(i, vertexBuffer);

			UINT8* srcVertBufferData = meshData.getStreamData(i);
			UINT8* vertBufferData = static_cast<UINT8*>(vertexBuffer->lock(GBL_WRITE_ONLY));

			UINT32 bufferSize = meshData.getStreamSize(i);

			memcpy(vertBufferData, srcVertBufferData, bufferSize);

			if(vertexBuffer->vertexColorReqRGBFlip())
			{
				UINT32 vertexStride = meshData.getVertexDesc()->getVertexStride(i);
				for(INT32 semanticIdx = 0; semanticIdx < VertexBuffer::MAX_SEMANTIC_IDX; semanticIdx++)
				{
					if(!meshData.getVertexDesc()->hasElement(VES_COLOR, semanticIdx, i))
						continue;

					UINT8* colorData = vertBufferData + meshData.getElementOffset(VES_COLOR, semanticIdx, i);
					for(UINT32 j = 0; j < mVertexData->vertexCount; j++)
					{
						UINT32* curColor = (UINT32*)colorData;

						(*curColor) = ((*curColor) & 0xFF00FF00) | ((*curColor >> 16) & 0x000000FF) | ((*curColor << 16) & 0x00FF0000);

						colorData += vertexStride;
					}
				}
			}

			vertexBuffer->unlock();
		}

		// Submeshes
		mSubMeshes.clear();

		if(meshData.getNumSubmeshes() > 0)
		{
			for(UINT32 i = 0; i < meshData.getNumSubmeshes(); i++)
			{
				UINT32 numIndices = meshData.getNumIndices(i);

				if(numIndices > 0)
				{
					mSubMeshes.push_back(SubMesh(meshData.getIndexBufferOffset(i), numIndices, meshData.getDrawOp(i), mVertexData, mIndexData, true));
				}
			}
		}
		else // Read it all as one mesh
		{
			UINT32 numIndices = meshData.getNumIndices();

			if(numIndices > 0)
			{
				mSubMeshes.push_back(SubMesh(0, numIndices, meshData.getDrawOp(), mVertexData, mIndexData, true));
			}
		}
	}

	void Mesh::readSubresource(UINT32 subresourceIdx, GpuResourceData& data)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(data.getTypeId() != TID_MeshData)
			CM_EXCEPT(InvalidParametersException, "Invalid GpuResourceData type. Only MeshData is supported.");

		IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT;
		if(mIndexData)
			indexType = mIndexData->indexBuffer->getType();

		MeshData& meshData = static_cast<MeshData&>(data);

		if(mIndexData)
		{
			UINT8* idxData = static_cast<UINT8*>(mIndexData->indexBuffer->lock(GBL_READ_ONLY));
			UINT32 idxElemSize = mIndexData->indexBuffer->getIndexSize();

			for(UINT32 i = 0; i < mSubMeshes.size(); i++)
			{
				UINT8* indices = nullptr;

				if(indexType == IndexBuffer::IT_16BIT)
					indices = (UINT8*)meshData.getIndices16(i);
				else
					indices = (UINT8*)meshData.getIndices32(i);

				memcpy(indices, &idxData[mSubMeshes[i].indexOffset * idxElemSize], mSubMeshes[i].indexCount * idxElemSize);
			}

			mIndexData->indexBuffer->unlock();
		}

		if(mVertexData)
		{
			auto vertexBuffers = mVertexData->getBuffers();

			UINT32 streamIdx = 0;
			for(auto iter = vertexBuffers.begin(); iter != vertexBuffers.end() ; ++iter)
			{
				VertexBufferPtr vertexBuffer = iter->second;
				UINT32 bufferSize = vertexBuffer->getVertexSize() * vertexBuffer->getNumVertices();
				UINT8* vertDataPtr = static_cast<UINT8*>(vertexBuffer->lock(GBL_READ_ONLY));

				UINT8* dest = meshData.getStreamData(streamIdx);
				memcpy(dest, vertDataPtr, bufferSize);

				vertexBuffer->unlock();

				streamIdx++;
			}
		}
	}

	MeshDataPtr Mesh::allocateSubresourceBuffer(UINT32 subresourceIdx) const
	{
		IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT;
		if(mIndexData)
			indexType = mIndexData->indexBuffer->getType();

		UINT32 numIndices = 0;
		if(mIndexData)
		{
			for(UINT32 i = 0; i < mSubMeshes.size(); i++)
				numIndices += mSubMeshes[i].indexCount;
		}

		VertexDataDescPtr vertexDesc = cm_shared_ptr<VertexDataDesc>();

		if(mVertexData)
		{
			auto vertexBuffers = mVertexData->getBuffers();

			UINT32 streamIdx = 0;
			for(auto iter = vertexBuffers.begin(); iter != vertexBuffers.end() ; ++iter)
			{
				VertexBufferPtr vertexBuffer = iter->second;
				UINT32 vertexSize = vertexBuffer->getVertexSize();

				UINT32 numElements = mVertexData->vertexDeclaration->getElementCount();
				for(UINT32 j = 0; j < numElements; j++)
				{
					const VertexElement* element = mVertexData->vertexDeclaration->getElement(j);
					VertexElementType type = element->getType();
					VertexElementSemantic semantic = element->getSemantic(); 
					UINT32 semanticIdx = element->getSemanticIdx();
					UINT32 offset = element->getOffset();
					UINT32 elemSize = element->getSize();

					vertexDesc->addVertElem(type, semantic, semanticIdx, streamIdx);
				}

				streamIdx++;
			}
		}

		MeshDataPtr meshData = cm_shared_ptr<MeshData>(mVertexData->vertexCount, numIndices, vertexDesc, DOT_TRIANGLE_LIST, indexType);

		if(mIndexData)
		{
			for(UINT32 i = 0; i < mSubMeshes.size(); i++)
				meshData->addSubMesh(mSubMeshes[i].indexCount, i);
		}

		return meshData;
	}

	const SubMesh& Mesh::getSubMesh(UINT32 subMeshIdx) const
	{
		THROW_IF_NOT_CORE_THREAD;

		if(subMeshIdx < 0 || subMeshIdx >= mSubMeshes.size())
		{
			CM_EXCEPT(InvalidParametersException, "Invalid sub-mesh index (" 
				+ toString(subMeshIdx) + "). Number of sub-meshes available: " + toString((int)mSubMeshes.size()));
		}

		// TODO - BIG TODO - Completely ignores subMeshIdx and always renders the entire thing because all submeshes
		// will share the same buffers

		return mSubMeshes[subMeshIdx];
	}

	const AABox& Mesh::getBounds() const
	{
		// TODO - Retrieve bounds for entire mesh (need to calculate them during creation)
		return AABox::BOX_EMPTY;
	}

	const AABox& Mesh::getBounds(UINT32 submeshIdx) const
	{
		// TODO - Retrieve bounds a specific sub-mesh (need to calculate them during creation)
		return AABox::BOX_EMPTY;
	}

	void Mesh::initialize_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		// TODO Low priority - Initialize an empty mesh. A better way would be to only initialize the mesh
		// once we set the proper mesh data (then we don't have to do it twice), but this makes the code less complex.
		// Consider changing it if there are performance issues.
		writeSubresource(0, *MeshManager::instance().getDummyMeshData());

		Resource::initialize_internal();
	}

	void Mesh::destroy_internal()
	{
		THROW_IF_NOT_CORE_THREAD;

		Resource::destroy_internal();
	}

	HMesh Mesh::dummy()
	{
		return MeshManager::instance().getDummyMesh();
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* Mesh::getRTTIStatic()
	{
		return MeshRTTI::instance();
	}

	RTTITypeBase* Mesh::getRTTI() const
	{
		return Mesh::getRTTIStatic();
	}

	/************************************************************************/
	/* 								STATICS		                     		*/
	/************************************************************************/

	HMesh Mesh::create()
	{
		MeshPtr meshPtr = MeshManager::instance().create();

		return static_resource_cast<Mesh>(Resource::_createResourceHandle(meshPtr));
	}
}