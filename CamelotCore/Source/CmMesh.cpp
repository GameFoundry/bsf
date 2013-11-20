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
	Mesh::Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		MeshBufferType bufferType, IndexBuffer::IndexType indexType)
		:mVertexData(nullptr), mIndexData(nullptr), mNumVertices(numVertices), mNumIndices(numIndices), 
		mVertexDesc(vertexDesc), mBufferType(bufferType), mIndexType(indexType)
	{ }

	Mesh::Mesh(const MeshDataPtr& initialMeshData, MeshBufferType bufferType)
		:mVertexData(nullptr), mIndexData(nullptr), mNumVertices(initialMeshData->getNumVertices()), 
		mNumIndices(initialMeshData->getNumIndices()), mBufferType(bufferType), mIndexType(initialMeshData->getIndexType()),
		mVertexDesc(initialMeshData->getVertexDesc()), mTempInitialMeshData(initialMeshData)
	{ }

	Mesh::Mesh()
		:mVertexData(nullptr), mIndexData(nullptr), mNumVertices(0), mNumIndices(0), mBufferType(MeshBufferType::Static), mIndexType(IndexBuffer::IT_32BIT)
	{ }

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
				LOGWRN("Buffer discard was set but buffer was created as static.");
				discardEntireBuffer = false;
			}
		}
		else
		{
			if(mBufferType == MeshBufferType::Dynamic)
			{
				LOGWRN("Buffer discard was not set but buffer was created as dynamic.");
				discardEntireBuffer = true;
			}
		}

		const MeshData& meshData = static_cast<const MeshData&>(data);

		// Indices
		UINT32 indexOffset = meshData.getResourceIndexOffset() * meshData.getIndexElementSize();
		UINT32 indicesSize = meshData.getIndexBufferSize();
		UINT8* srcIdxData = meshData.getIndexData(); 

		if((indexOffset + indicesSize) > mIndexData->indexBuffer->getSizeInBytes())
			CM_EXCEPT(InvalidParametersException, "Index buffer values are being written out of valid range.");

		mIndexData->indexBuffer->writeData(indexOffset, indicesSize, srcIdxData, discardEntireBuffer);

		// Vertices
		for(UINT32 i = 0; i <= meshData.getVertexDesc()->getMaxStreamIdx(); i++)
		{
			if(!meshData.getVertexDesc()->hasStream(i))
				continue;

			if(i >= mVertexData->getBufferCount())
				CM_EXCEPT(InvalidParametersException, "Attempting to write to a vertex stream that doesn't exist on this mesh.");

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

					UINT8* colorData = bufferCopy + meshData.getElementOffset(VES_COLOR, semanticIdx, i);
					for(UINT32 j = 0; j < mVertexData->vertexCount; j++)
					{
						UINT32* curColor = (UINT32*)colorData;

						(*curColor) = ((*curColor) & 0xFF00FF00) | ((*curColor >> 16) & 0x000000FF) | ((*curColor << 16) & 0x00FF0000);

						colorData += vertexStride;
					}
				}

				vertexBuffer->writeData(bufferOffset, bufferSize, bufferCopy, discardEntireBuffer);

				cm_free(bufferCopy);
			}
			else
			{
				vertexBuffer->writeData(bufferOffset, bufferSize, srcVertBufferData, discardEntireBuffer);
			}
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
			UINT32 indexResourceOffset = meshData.getResourceIndexOffset() * meshData.getIndexElementSize();

			for(UINT32 i = 0; i < mSubMeshes.size(); i++)
			{
				UINT8* indices = nullptr;

				if(indexType == IndexBuffer::IT_16BIT)
					indices = (UINT8*)meshData.getIndices16(i);
				else
					indices = (UINT8*)meshData.getIndices32(i);

				UINT32 indicesSize = mSubMeshes[i].indexCount * idxElemSize;
				UINT32 indicesOffset = meshData.getIndexBufferOffset(i) + indexResourceOffset;
				
				if((indicesOffset + indicesSize) > meshData.getIndexBufferSize())
					CM_EXCEPT(InvalidParametersException, "Provided buffer doesn't have enough space to store mesh indices.");

				indices += indexResourceOffset;
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
				if(streamIdx > meshData.getVertexDesc()->getMaxStreamIdx())
					continue;

				VertexBufferPtr vertexBuffer = iter->second;
				UINT32 bufferSize = vertexBuffer->getVertexSize() * vertexBuffer->getNumVertices();
				
				UINT32 vertexResourceOffset = meshData.getResourceVertexOffset() * meshData.getVertexDesc()->getVertexStride(streamIdx);
				UINT32 vertexOffset = meshData.getStreamOffset(streamIdx) + vertexResourceOffset;

				UINT8* vertDataPtr = static_cast<UINT8*>(vertexBuffer->lock(GBL_READ_ONLY));

				if((vertexOffset + bufferSize) > meshData.getStreamSize(streamIdx))
					CM_EXCEPT(InvalidParametersException, "Provided buffer doesn't have enough space to store mesh vertices.");

				UINT8* dest = meshData.getStreamData(streamIdx) + vertexResourceOffset;
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

		MeshDataPtr meshData = cm_shared_ptr<MeshData>(mVertexData->vertexCount, numIndices, mVertexDesc, DOT_TRIANGLE_LIST, indexType);

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

	HMesh Mesh::create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, MeshBufferType bufferType, IndexBuffer::IndexType indexType)
	{
		MeshPtr meshPtr = MeshManager::instance().create(numVertices, numIndices, vertexDesc, bufferType, indexType);

		return static_resource_cast<Mesh>(Resource::_createResourceHandle(meshPtr));
	}

	HMesh Mesh::create(const MeshDataPtr& initialMeshData, MeshBufferType bufferType)
	{
		MeshPtr meshPtr = MeshManager::instance().create(initialMeshData, bufferType);

		return static_resource_cast<Mesh>(Resource::_createResourceHandle(meshPtr));
	}
}