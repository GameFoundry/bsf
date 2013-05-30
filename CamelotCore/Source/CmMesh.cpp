#include "CmMesh.h"
#include "CmMeshRTTI.h"
#include "CmMeshData.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmDebug.h"
#include "CmHardwareBufferManager.h"
#include "CmMeshManager.h"
#include "CmRenderSystem.h"
#include "CmAsyncOp.h"

#if CM_DEBUG_MODE
#define THROW_IF_NOT_RENDER_THREAD throwIfNotRenderThread();
#else
#define THROW_IF_NOT_RENDER_THREAD 
#endif

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
		THROW_IF_NOT_RENDER_THREAD;

		if(data.getTypeId() != TID_MeshData)
			CM_EXCEPT(InvalidParametersException, "Invalid GpuResourceData type. Only MeshData is supported.");

		const MeshData& meshData = static_cast<const MeshData&>(data);

		mSubMeshes.clear();

		if(mVertexData != nullptr)
			cm_delete<PoolAlloc>(mVertexData);

		if(mIndexData != nullptr)
			cm_delete<PoolAlloc>(mIndexData);

		// Submeshes
		for(UINT32 i = 0; i < meshData.getNumSubmeshes(); i++)
		{
			UINT32 numIndices = meshData.getNumIndices(i);

			if(numIndices > 0)
			{
				mSubMeshes.push_back(SubMesh(meshData.getIndexBufferOffset(i), numIndices));
			}
		}

		// Indices
		mIndexData = cm_new<IndexData, PoolAlloc>();

		mIndexData->indexCount = meshData.getNumIndices();
		mIndexData->indexBuffer = HardwareBufferManager::instance().createIndexBuffer(
			meshData.getIndexType(),
			mIndexData->indexCount, 
			GBU_STATIC);

		UINT8* idxData = static_cast<UINT8*>(mIndexData->indexBuffer->lock(GBL_WRITE_ONLY_DISCARD));
		UINT32 idxElementSize = meshData.getIndexElementSize();

		UINT32 indicesSize = meshData.getIndexBufferSize();
		UINT8* srcIdxData = meshData.getIndexData(); 

		memcpy(idxData, srcIdxData, indicesSize);

		mIndexData->indexBuffer->unlock();

		// Vertices
		mVertexData = cm_new<VertexData, PoolAlloc>();

		mVertexData->vertexCount = meshData.getNumVertices();
		mVertexData->vertexDeclaration = meshData.createDeclaration();

		for(UINT32 i = 0; i <= meshData.getMaxStreamIdx(); i++)
		{
			if(!meshData.hasStream(i))
				continue;

			UINT32 streamSize = meshData.getStreamSize(i);

			VertexBufferPtr vertexBuffer = HardwareBufferManager::instance().createVertexBuffer(
				mVertexData->vertexDeclaration->getVertexSize(i),
				mVertexData->vertexCount,
				GBU_STATIC);

			mVertexData->setBuffer(i, vertexBuffer);

			UINT8* srcVertBufferData = meshData.getStreamData(i);
			UINT8* vertBufferData = static_cast<UINT8*>(vertexBuffer->lock(GBL_WRITE_ONLY_DISCARD));

			UINT32 bufferSize = meshData.getStreamSize(i);

			memcpy(vertBufferData, srcVertBufferData, bufferSize);

			vertexBuffer->unlock();
		}
	}

	void Mesh::readSubresource(UINT32 subresourceIdx, GpuResourceData& data)
	{
		THROW_IF_NOT_RENDER_THREAD;

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

		MeshDataPtr meshData = cm_shared_ptr<MeshData, PoolAlloc>(mVertexData->vertexCount, indexType);

		meshData->beginDesc();
		if(mIndexData)
		{
			for(UINT32 i = 0; i < mSubMeshes.size(); i++)
				meshData->addSubMesh(mSubMeshes[i].indexCount, i);
		}

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

					meshData->addVertElem(type, semantic, semanticIdx, streamIdx);
				}

				streamIdx++;
			}
		}
		meshData->endDesc();

		return meshData;
	}

	RenderOperation Mesh::getRenderOperation(UINT32 subMeshIdx) const
	{
		if(subMeshIdx < 0 || subMeshIdx >= mSubMeshes.size())
		{
			CM_EXCEPT(InvalidParametersException, "Invalid sub-mesh index (" 
				+ toString(subMeshIdx) + "). Number of sub-meshes available: " + toString(mSubMeshes.size()));
		}

		// TODO - BIG TODO - Completely ignores subMeshIdx and always renders the entire thing
		RenderOperation ro;
		ro.indexData = mIndexData;
		ro.vertexData = mVertexData;
		ro.useIndexes = true;
		ro.operationType = DOT_TRIANGLE_LIST;

		return ro;
	}

	void Mesh::initialize_internal()
	{
		THROW_IF_NOT_RENDER_THREAD;

		// TODO Low priority - Initialize an empty mesh. A better way would be to only initialize the mesh
		// once we set the proper mesh data (then we don't have to do it twice), but this makes the code less complex.
		// Consider changing it if there are performance issues.
		writeSubresource(0, *MeshManager::instance().getDummyMeshData());

		Resource::initialize_internal();
	}

	void Mesh::destroy_internal()
	{
		THROW_IF_NOT_RENDER_THREAD;

		if(mVertexData != nullptr)
			cm_delete<PoolAlloc>(mVertexData);

		if(mIndexData != nullptr)
			cm_delete<PoolAlloc>(mIndexData);

		Resource::destroy_internal();
	}

	HMesh Mesh::dummy()
	{
		return MeshManager::instance().getDummyMesh();
	}

	void Mesh::throwIfNotRenderThread() const
	{
		if(CM_THREAD_CURRENT_ID != RenderSystem::instancePtr()->getRenderThreadId())
			CM_EXCEPT(InternalErrorException, "Calling an internal texture method from a non-render thread!");
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

#undef THROW_IF_NOT_RENDER_THREAD