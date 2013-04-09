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

namespace CamelotEngine
{
	Mesh::Mesh()
		:mVertexData(nullptr), mIndexData(nullptr)
	{

	}

	Mesh::~Mesh()
	{
	}

	void Mesh::setMeshData(MeshDataPtr meshData)
	{
		RenderSystem::instancePtr()->queueCommand(boost::bind(&Mesh::setMeshData_internal, this, meshData), true);
	}

	void Mesh::setMeshData_internal(MeshDataPtr meshData)
	{
		THROW_IF_NOT_RENDER_THREAD;

		if(meshData == nullptr)
		{
			CM_EXCEPT(InvalidParametersException, "Cannot load mesh. Mesh data is null.");
		}

		// Ensure all vertex elements are of proper size
		UINT32 numVertices = 0;

		auto vertElemDataPerStream = meshData->mVertexData;
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

		for(auto& vertElems : meshData->mVertexData)
		{
			for(auto& vertElem : vertElems.second)
			{
				if(vertElem.elementCount != numVertices)
				{
					CM_EXCEPT(InvalidParametersException, "All vertex element arrays in MeshData need to be of the same size. Found an array with semantic: "
						+ toString(vertElem.element.getSemantic()) + " and element count: " + toString(vertElem.elementCount) + ". This doesn't match with other "
						+ "element with semantic: " + toString(firstElemData->element.getSemantic()) + " and element count: " + toString(firstElemData->elementCount));
				}
			}
		}

		mSubMeshes.clear();

		if(mVertexData != nullptr)
			CM_DELETE(mVertexData, VertexData, PoolAlloc);

		if(mIndexData != nullptr)
			CM_DELETE(mIndexData, IndexData, PoolAlloc);

		// Submeshes
		UINT32 indexOffset = 0;
		UINT32 totalIndexCount = 0;
		for(auto& i : meshData->mIndices)
		{
			UINT32 numIndices = i.numIndices;

			if(numIndices > 0)
			{
				mSubMeshes.push_back(SubMesh(indexOffset, numIndices));
				indexOffset += numIndices;
				totalIndexCount += numIndices;
			}
		}

		// Indices
		mIndexData = CM_NEW(IndexData, PoolAlloc) IndexData();

		mIndexData->indexCount = totalIndexCount;
		mIndexData->indexBuffer = HardwareBufferManager::instance().createIndexBuffer(
			meshData->mIndexType,
			mIndexData->indexCount, 
			GBU_STATIC);

		UINT8* idxData = static_cast<UINT8*>(mIndexData->indexBuffer->lock(GBL_WRITE_ONLY));
		UINT32 idxElementSize = meshData->getIndexElementSize();

		indexOffset = 0;
		for(auto& i : meshData->mIndices)
		{
			memcpy(&idxData[indexOffset], i.indices, i.numIndices * idxElementSize);
			indexOffset += i.numIndices;
		}

		mIndexData->indexBuffer->unlock();

		// Vertices
		mVertexData = CM_NEW(VertexData, PoolAlloc) VertexData();

		mVertexData->vertexCount = numVertices;
		mVertexData->vertexDeclaration = meshData->createDeclaration();

		for(auto& vertElems : meshData->mVertexData)
		{
			UINT32 streamIdx = vertElems.first;

			if(vertElems.second.size() == 0)
				continue;

			VertexBufferPtr vertexBuffer = HardwareBufferManager::instance().createVertexBuffer(
				mVertexData->vertexDeclaration->getVertexSize(streamIdx),
				mVertexData->vertexCount,
				GBU_STATIC);

			mVertexData->setBuffer(streamIdx, vertexBuffer);

			UINT32 vertexSize = vertexBuffer->getVertexSize();
			UINT8* vertBufferData = static_cast<UINT8*>(vertexBuffer->lock(GBL_WRITE_ONLY));

			UINT32 numElements = mVertexData->vertexDeclaration->getElementCount();

			for(UINT32 j = 0; j < numElements; j++)
			{
				const VertexElement* element = mVertexData->vertexDeclaration->getElement(j);
				UINT32 offset = element->getOffset();
				UINT32 elemSize = element->getSize();

				MeshData::VertexElementData& elemData = meshData->getVertElemData(element->getType(), element->getSemantic(), element->getIndex(), streamIdx);

				UINT8* sourceData = elemData.data;
				for(UINT32 k = 0; k < elemData.elementCount; k++)
				{
					memcpy(&vertBufferData[k * vertexSize + offset], &sourceData[k * elemSize], elemSize);
				}
			}

			vertexBuffer->unlock();
		}
	}

	MeshDataPtr Mesh::getMeshData()
	{
		AsyncOp op = RenderSystem::instancePtr()->queueReturnCommand(boost::bind(&Mesh::getMeshData_internal, this, _1), true);

		return op.getReturnValue<MeshDataPtr>();
	}

	void Mesh::getMeshData_internal(AsyncOp& asyncOp)
	{
		IndexBuffer::IndexType indexType = IndexBuffer::IT_32BIT;
		if(mIndexData)
			indexType = mIndexData->indexBuffer->getType();

		MeshDataPtr meshData(CM_NEW(MeshData, PoolAlloc) MeshData(indexType),
			&MemAllocDeleter<MeshData, PoolAlloc>::deleter);

		if(mIndexData)
		{
			UINT8* idxData = static_cast<UINT8*>(mIndexData->indexBuffer->lock(GBL_READ_ONLY));
			UINT32 idxElemSize = mIndexData->indexBuffer->getIndexSize();

			for(UINT32 i = 0; i < mSubMeshes.size(); i++)
			{
				UINT8* indices = nullptr;
				
				if(indexType == IndexBuffer::IT_16BIT)
					indices = (UINT8*)meshData->addIndices16(mSubMeshes[i].indexCount, i);
				else
					indices = (UINT8*)meshData->addIndices32(mSubMeshes[i].indexCount, i);
				
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
				UINT32 vertexSize = vertexBuffer->getVertexSize();
				UINT8* vertDataIter = static_cast<UINT8*>(vertexBuffer->lock(GBL_READ_ONLY));

				UINT32 numElements = mVertexData->vertexDeclaration->getElementCount();
				for(UINT32 j = 0; j < numElements; j++)
				{
					const VertexElement* element = mVertexData->vertexDeclaration->getElement(j);
					VertexElementType type = element->getType();
					VertexElementSemantic semantic = element->getSemantic(); 
					UINT32 semanticIdx = element->getIndex();
					UINT32 offset = element->getOffset();
					UINT32 elemSize = element->getSize();

					UINT8* dest = meshData->addVertexElementData(type, semantic, mVertexData->vertexCount, semanticIdx, streamIdx);
					for(UINT32 k = 0; k < mVertexData->vertexCount; k++)
						memcpy(&dest[k * elemSize], &vertDataIter[k * vertexSize + offset], elemSize);
				}

				vertexBuffer->unlock();

				streamIdx++;
			}
		}

		asyncOp.completeOperation(meshData);
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
		setMeshData_internal(MeshManager::instance().getNullMeshData());

		Resource::initialize_internal();
	}

	void Mesh::destroy_internal()
	{
		THROW_IF_NOT_RENDER_THREAD;

		if(mVertexData != nullptr)
			CM_DELETE(mVertexData, VertexData, PoolAlloc);

		if(mIndexData != nullptr)
			CM_DELETE(mIndexData, IndexData, PoolAlloc);

		Resource::destroy_internal();
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