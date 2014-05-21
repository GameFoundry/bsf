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
#include "CmResources.h"

#include "CmProfiler.h"

namespace BansheeEngine
{
	Mesh::Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		MeshBufferType bufferType, DrawOperationType drawOp, IndexBuffer::IndexType indexType)
		:MeshBase(numVertices, numIndices, drawOp), mVertexData(nullptr), mIndexData(nullptr),
		mVertexDesc(vertexDesc), mBufferType(bufferType), mIndexType(indexType)
	{

	}

	Mesh::Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp, IndexBuffer::IndexType indexType)
		:MeshBase(numVertices, numIndices, drawOp), mVertexData(nullptr), mIndexData(nullptr),
		mVertexDesc(vertexDesc), mBufferType(bufferType), mIndexType(indexType), 
		mTempInitialMeshData(initialMeshData)
	{

	}

	Mesh::Mesh(const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp)
		:MeshBase(initialMeshData->getNumVertices(), initialMeshData->getNumIndices(), drawOp), 
		mVertexData(nullptr), mIndexData(nullptr), mIndexType(initialMeshData->getIndexType()),
		mVertexDesc(initialMeshData->getVertexDesc()), mTempInitialMeshData(initialMeshData)
	{

	}

	Mesh::Mesh()
		:MeshBase(0, 0, DOT_TRIANGLE_LIST), mVertexData(nullptr), mIndexData(nullptr), 
		mBufferType(MeshBufferType::Static), mIndexType(IndexBuffer::IT_32BIT)
	{

	}

	Mesh::~Mesh()
	{
	}

	void Mesh::writeSubresource(UINT32 subresourceIdx, const GpuResourceData& data, bool discardEntireBuffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(data.getTypeId() != TID_MeshData)
			CM_EXCEPT(InvalidParametersException, "Invalid GpuResourceData type. Only MeshData is supported.");

		if(discardEntireBuffer)
		{
			if(mBufferType == MeshBufferType::Static)
			{
				LOGWRN("Buffer discard is enabled but buffer was not created as dynamic. Disabling discard.");
				discardEntireBuffer = false;
			}
		}
		else
		{
			if(mBufferType == MeshBufferType::Dynamic)
			{
				LOGWRN("Buffer discard is not enabled but buffer was created as dynamic. Enabling discard.");
				discardEntireBuffer = true;
			}
		}

		const MeshData& meshData = static_cast<const MeshData&>(data);

		// Indices
		UINT32 indexOffset = meshData.getResourceIndexOffset() * meshData.getIndexElementSize();
		UINT32 indicesSize = meshData.getIndexBufferSize();
		UINT8* srcIdxData = meshData.getIndexData(); 

		if(meshData.getIndexElementSize() != mIndexData->indexBuffer->getIndexSize())
		{
			CM_EXCEPT(InvalidParametersException, "Provided index size doesn't match meshes index size. Needed: " + 
				toString(mIndexData->indexBuffer->getIndexSize()) + ". Got: " + toString(meshData.getIndexElementSize()));
		}

		if((indexOffset + indicesSize) > mIndexData->indexBuffer->getSizeInBytes())
			CM_EXCEPT(InvalidParametersException, "Index buffer values are being written out of valid range.");

		mIndexData->indexBuffer->writeData(indexOffset, indicesSize, srcIdxData, discardEntireBuffer ? BufferWriteType::Discard : BufferWriteType::Normal);

		// Vertices
		for(UINT32 i = 0; i <= mVertexDesc->getMaxStreamIdx(); i++)
		{
			if(!mVertexDesc->hasStream(i))
				continue;

			if(!meshData.getVertexDesc()->hasStream(i))
				continue;

			// Ensure both have the same sized vertices
			UINT32 myVertSize = mVertexDesc->getVertexStride(i);
			UINT32 otherVertSize = meshData.getVertexDesc()->getVertexStride(i);
			if(myVertSize != otherVertSize)
			{
				CM_EXCEPT(InvalidParametersException, "Provided vertex size for stream " + toString(i) + " doesn't match meshes vertex size. Needed: " + 
					toString(myVertSize) + ". Got: " + toString(otherVertSize));
			}

			VertexBufferPtr vertexBuffer = mVertexData->getBuffer(i);

			UINT32 bufferOffset = meshData.getResourceVertexOffset() * meshData.getVertexDesc()->getVertexStride(i);
			UINT32 bufferSize = meshData.getStreamSize(i);
			UINT8* srcVertBufferData = meshData.getStreamData(i);

			if((bufferOffset + bufferSize) > vertexBuffer->getSizeInBytes())
				CM_EXCEPT(InvalidParametersException, "Vertex buffer values for stream \"" + toString(i) + "\" are being written out of valid range.");

			if(vertexBuffer->vertexColorReqRGBFlip())
			{
				UINT8* bufferCopy = (UINT8*)cm_alloc(bufferSize);
				memcpy(bufferCopy, srcVertBufferData, bufferSize); // TODO Low priority - Attempt to avoid this copy

				UINT32 vertexStride = meshData.getVertexDesc()->getVertexStride(i);
				for(INT32 semanticIdx = 0; semanticIdx < VertexBuffer::MAX_SEMANTIC_IDX; semanticIdx++)
				{
					if(!meshData.getVertexDesc()->hasElement(VES_COLOR, semanticIdx, i))
						continue;

					UINT8* colorData = bufferCopy + mVertexDesc->getElementOffsetFromStream(VES_COLOR, semanticIdx, i);
					for(UINT32 j = 0; j < mVertexData->vertexCount; j++)
					{
						UINT32* curColor = (UINT32*)colorData;

						(*curColor) = ((*curColor) & 0xFF00FF00) | ((*curColor >> 16) & 0x000000FF) | ((*curColor << 16) & 0x00FF0000);

						colorData += vertexStride;
					}
				}

				vertexBuffer->writeData(bufferOffset, bufferSize, bufferCopy, discardEntireBuffer ? BufferWriteType::Discard : BufferWriteType::Normal);

				cm_free(bufferCopy);
			}
			else
			{
				vertexBuffer->writeData(bufferOffset, bufferSize, srcVertBufferData, discardEntireBuffer ? BufferWriteType::Discard : BufferWriteType::Normal);
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
			if(meshData.getIndexElementSize() != mIndexData->indexBuffer->getIndexSize())
			{
				CM_EXCEPT(InvalidParametersException, "Provided index size doesn't match meshes index size. Needed: " + 
					toString(mIndexData->indexBuffer->getIndexSize()) + ". Got: " + toString(meshData.getIndexElementSize()));
			}

			UINT8* idxData = static_cast<UINT8*>(mIndexData->indexBuffer->lock(GBL_READ_ONLY));
			UINT32 idxElemSize = mIndexData->indexBuffer->getIndexSize();
			UINT32 indexResourceOffset = meshData.getResourceIndexOffset();

			UINT8* indices = nullptr;

			if(indexType == IndexBuffer::IT_16BIT)
				indices = (UINT8*)meshData.getIndices16();
			else
				indices = (UINT8*)meshData.getIndices32();

			UINT32 remainingNumIndices = (UINT32)std::max(0, (INT32)(mNumIndices - indexResourceOffset));
			UINT32 numIndicesToCopy = std::min(remainingNumIndices, meshData.getNumIndices());

			UINT32 indicesSize = numIndicesToCopy * idxElemSize;
			if(indicesSize > meshData.getIndexBufferSize())
				CM_EXCEPT(InvalidParametersException, "Provided buffer doesn't have enough space to store mesh indices.");

			idxData += indexResourceOffset * idxElemSize;
			memcpy(indices, idxData, numIndicesToCopy * idxElemSize);

			mIndexData->indexBuffer->unlock();
		}

		if(mVertexData)
		{
			auto vertexBuffers = mVertexData->getBuffers();
			
			UINT32 streamIdx = 0;
			for(auto iter = vertexBuffers.begin(); iter != vertexBuffers.end() ; ++iter)
			{
				if(!meshData.getVertexDesc()->hasStream(streamIdx))
					continue;

				// Ensure both have the same sized vertices
				UINT32 myVertSize = mVertexDesc->getVertexStride(streamIdx);
				UINT32 otherVertSize = meshData.getVertexDesc()->getVertexStride(streamIdx);
				if(myVertSize != otherVertSize)
				{
					CM_EXCEPT(InvalidParametersException, "Provided vertex size for stream " + toString(streamIdx) + " doesn't match meshes vertex size. Needed: " + 
						toString(myVertSize) + ". Got: " + toString(otherVertSize));
				}

				UINT32 vertexResourceOffset = meshData.getResourceVertexOffset();
				UINT32 bufferOffset = vertexResourceOffset * meshData.getVertexDesc()->getVertexStride(streamIdx);

				UINT32 numVerticesToCopy = meshData.getNumVertices();

				VertexBufferPtr vertexBuffer = iter->second;
				UINT32 bufferSize = vertexBuffer->getVertexSize() * numVerticesToCopy;
				
				if((bufferOffset + bufferSize) > vertexBuffer->getSizeInBytes())
					CM_EXCEPT(InvalidParametersException, "Vertex buffer values for stream \"" + toString(streamIdx) + "\" are being read out of valid range.");

				UINT8* vertDataPtr = static_cast<UINT8*>(vertexBuffer->lock(GBL_READ_ONLY)) + vertexResourceOffset * meshData.getVertexDesc()->getVertexStride(streamIdx);

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

		MeshDataPtr meshData = cm_shared_ptr<MeshData>(mVertexData->vertexCount, mNumIndices, mVertexDesc, indexType);

		return meshData;
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

	std::shared_ptr<VertexData> Mesh::_getVertexData() const
	{
		THROW_IF_NOT_CORE_THREAD;

		return mVertexData;
	}

	std::shared_ptr<IndexData> Mesh::_getIndexData() const
	{
		THROW_IF_NOT_CORE_THREAD;

		return mIndexData;
	}

	void Mesh::initialize_internal()
	{
		THROW_IF_NOT_CORE_THREAD;
		
		mIndexData = std::shared_ptr<IndexData>(cm_new<IndexData, PoolAlloc>());

		mIndexData->indexCount = mNumIndices;
		mIndexData->indexBuffer = HardwareBufferManager::instance().createIndexBuffer(
			mIndexType,
			mIndexData->indexCount, 
			mBufferType == MeshBufferType::Dynamic ? GBU_DYNAMIC : GBU_STATIC);

		mVertexData = std::shared_ptr<VertexData>(cm_new<VertexData, PoolAlloc>());

		mVertexData->vertexCount = mNumVertices;
		mVertexData->vertexDeclaration = mVertexDesc->createDeclaration();

		for(UINT32 i = 0; i <= mVertexDesc->getMaxStreamIdx(); i++)
		{
			if(!mVertexDesc->hasStream(i))
				continue;

			VertexBufferPtr vertexBuffer = HardwareBufferManager::instance().createVertexBuffer(
				mVertexData->vertexDeclaration->getVertexSize(i),
				mVertexData->vertexCount,
				mBufferType == MeshBufferType::Dynamic ? GBU_DYNAMIC : GBU_STATIC);

			mVertexData->setBuffer(i, vertexBuffer);
		}

		// TODO Low priority - DX11 (and maybe OpenGL)? allow an optimization that allows you to set
		// buffer data upon buffer construction, instead of setting it in a second step like I do here
		if(mTempInitialMeshData != nullptr)
		{
			writeSubresource(0, *mTempInitialMeshData, mBufferType == MeshBufferType::Dynamic);

			mTempInitialMeshData = nullptr;
		}

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

	HMesh Mesh::create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		MeshBufferType bufferType, DrawOperationType drawOp, IndexBuffer::IndexType indexType)
	{
		MeshPtr meshPtr = _createPtr(numVertices, numIndices, vertexDesc, bufferType, drawOp, indexType);

		return static_resource_cast<Mesh>(gResources()._createResourceHandle(meshPtr));
	}

	HMesh Mesh::create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp, IndexBuffer::IndexType indexType)
	{
		MeshPtr meshPtr = _createPtr(numVertices, numIndices, vertexDesc, 
			initialMeshData, bufferType, drawOp, indexType);

		return static_resource_cast<Mesh>(gResources()._createResourceHandle(meshPtr));
	}

	HMesh Mesh::create(const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp)
	{
		MeshPtr meshPtr = _createPtr(initialMeshData, bufferType, drawOp);

		return static_resource_cast<Mesh>(gResources()._createResourceHandle(meshPtr));
	}

	MeshPtr Mesh::_createPtr(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		MeshBufferType bufferType, DrawOperationType drawOp, IndexBuffer::IndexType indexType)
	{
		return MeshManager::instance().create(numVertices, numIndices, vertexDesc, bufferType, drawOp, indexType);
	}

	MeshPtr Mesh::_createPtr(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp, IndexBuffer::IndexType indexType)
	{
		return MeshManager::instance().create(numVertices, numIndices, vertexDesc, 
			initialMeshData, bufferType, drawOp, indexType);
	}

	MeshPtr Mesh::_createPtr(const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp)
	{
		return MeshManager::instance().create(initialMeshData, bufferType, drawOp);
	}
}