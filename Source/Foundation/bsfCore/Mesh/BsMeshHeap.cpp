//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Mesh/BsMeshHeap.h"
#include "CoreThread/BsCoreThread.h"
#include "Mesh/BsTransientMesh.h"
#include "Managers/BsHardwareBufferManager.h"
#include "RenderAPI/BsVertexDataDesc.h"
#include "RenderAPI/BsVertexData.h"
#include "Mesh/BsMeshData.h"
#include "Math/BsMath.h"
#include "RenderAPI/BsEventQuery.h"
#include "RenderAPI/BsRenderAPI.h"

namespace bs
{
	MeshHeap::MeshHeap(UINT32 numVertices, UINT32 numIndices,
		const SPtr<VertexDataDesc>& vertexDesc, IndexType indexType)
		:mNumVertices(numVertices), mNumIndices(numIndices), mVertexDesc(vertexDesc), mIndexType(indexType), mNextFreeId(0)
	{
	}

	SPtr<MeshHeap> MeshHeap::create(UINT32 numVertices, UINT32 numIndices, const SPtr<VertexDataDesc>& vertexDesc,
		IndexType indexType)
	{
		MeshHeap* meshHeap = new (bs_alloc<MeshHeap>()) MeshHeap(numVertices, numIndices, vertexDesc, indexType);
		SPtr<MeshHeap> meshHeapPtr = bs_core_ptr<MeshHeap>(meshHeap);

		meshHeapPtr->_setThisPtr(meshHeapPtr);
		meshHeapPtr->initialize();

		return meshHeapPtr;
	}

	SPtr<TransientMesh> MeshHeap::alloc(const SPtr<MeshData>& meshData, DrawOperationType drawOp)
	{
		UINT32 meshIdx = mNextFreeId++;

		SPtr<MeshHeap> thisPtr = std::static_pointer_cast<MeshHeap>(getThisPtr());
		TransientMesh* transientMesh = new (bs_alloc<TransientMesh>()) TransientMesh(thisPtr, meshIdx,
			meshData->getNumVertices(), meshData->getNumIndices(), drawOp);
		SPtr<TransientMesh> transientMeshPtr = bs_core_ptr<TransientMesh>(transientMesh);

		transientMeshPtr->_setThisPtr(transientMeshPtr);
		transientMeshPtr->initialize();

		mMeshes[meshIdx] = transientMeshPtr;

		queueGpuCommand(getCore(), std::bind(&ct::MeshHeap::alloc, getCore().get(), transientMeshPtr->getCore(), meshData));

		return transientMeshPtr;
	}

	void MeshHeap::dealloc(const SPtr<TransientMesh>& mesh)
	{
		auto iterFind = mMeshes.find(mesh->mId);
		if(iterFind == mMeshes.end())
			return;

		mesh->markAsDestroyed();
		mMeshes.erase(iterFind);

		queueGpuCommand(getCore(), std::bind(&ct::MeshHeap::dealloc, getCore().get(), mesh->getCore()));
	}

	SPtr<ct::MeshHeap> MeshHeap::getCore() const
	{
		return std::static_pointer_cast<ct::MeshHeap>(mCoreSpecific);
	}

	SPtr<ct::CoreObject> MeshHeap::createCore() const
	{
		ct::MeshHeap* obj = new (bs_alloc<ct::MeshHeap>()) ct::MeshHeap(mNumVertices, mNumIndices,
			mVertexDesc, mIndexType, GDF_DEFAULT);

		SPtr<ct::MeshHeap> corePtr = bs_shared_ptr<ct::MeshHeap>(obj);
		obj->_setThisPtr(corePtr);

		return corePtr;
	}

	namespace ct
	{
	const float MeshHeap::GrowPercent = 1.5f;

	MeshHeap::MeshHeap(UINT32 numVertices, UINT32 numIndices,
		const SPtr<VertexDataDesc>& vertexDesc, IndexType indexType, GpuDeviceFlags deviceMask)
		: mNumVertices(numVertices), mNumIndices(numIndices), mCPUIndexData(nullptr), mVertexDesc(vertexDesc)
		, mIndexType(indexType), mDeviceMask(deviceMask), mNextQueryId(0)
	{
		for (UINT32 i = 0; i <= mVertexDesc->getMaxStreamIdx(); i++)
		{
			mCPUVertexData.push_back(nullptr);
		}
	}

	MeshHeap::~MeshHeap()
	{
		THROW_IF_NOT_CORE_THREAD;

		for (auto& cpuVertBuffer : mCPUVertexData)
			bs_free(cpuVertBuffer);

		if (mCPUIndexData != nullptr)
			bs_free(mCPUIndexData);

		mVertexData = nullptr;
		mIndexBuffer = nullptr;
		mVertexDesc = nullptr;
	}

	void MeshHeap::initialize()
	{
		THROW_IF_NOT_CORE_THREAD;

		growVertexBuffer(mNumVertices);
		growIndexBuffer(mNumIndices);

		CoreObject::initialize();
	}

	void MeshHeap::alloc(SPtr<TransientMesh> mesh, const SPtr<MeshData>& meshData)
	{
		// Find free vertex chunk and grow if needed
		UINT32 smallestVertFit = 0;
		UINT32 smallestVertFitIdx = 0;

		while (smallestVertFit == 0)
		{
			UINT32 curIdx = 0;
			for (auto& chunkIdx : mFreeVertChunks)
			{
				ChunkData& chunk = mVertChunks[chunkIdx];

				if (chunk.size >= meshData->getNumVertices() && (chunk.size < smallestVertFit || smallestVertFit == 0))
				{
					smallestVertFit = chunk.size;
					smallestVertFitIdx = curIdx;
				}

				curIdx++;
			}

			if (smallestVertFit > 0)
				break;

			UINT32 newNumVertices = mNumVertices;
			while (newNumVertices < (mNumVertices + meshData->getNumVertices()))
			{
				newNumVertices = Math::roundToInt(newNumVertices * GrowPercent);
			}

			growVertexBuffer(newNumVertices);
		}

		// Find free index chunk and grow if needed
		UINT32 smallestIdxFit = 0;
		UINT32 smallestIdxFitIdx = 0;

		while (smallestIdxFit == 0)
		{
			UINT32 curIdx = 0;
			for (auto& chunkIdx : mFreeIdxChunks)
			{
				ChunkData& chunk = mIdxChunks[chunkIdx];

				if (chunk.size >= meshData->getNumIndices() && (chunk.size < smallestIdxFit || smallestIdxFit == 0))
				{
					smallestIdxFit = chunk.size;
					smallestIdxFitIdx = curIdx;
				}

				curIdx++;
			}

			if (smallestIdxFit > 0)
				break;

			UINT32 newNumIndices = mNumIndices;
			while (newNumIndices < (mNumIndices + meshData->getNumIndices()))
			{
				newNumIndices = Math::roundToInt(newNumIndices * GrowPercent);
			}

			growIndexBuffer(newNumIndices);
		}

		UINT32 freeVertChunkIdx = 0;
		UINT32 freeIdxChunkIdx = 0;

		auto freeVertIter = mFreeVertChunks.begin();
		freeVertChunkIdx = (*freeVertIter);
		for (UINT32 i = 0; i < smallestVertFitIdx; i++)
		{
			freeVertIter++;
			freeVertChunkIdx = (*freeVertIter);
		}

		mFreeVertChunks.erase(freeVertIter);

		auto freeIdxIter = mFreeIdxChunks.begin();
		freeIdxChunkIdx = (*freeIdxIter);
		for (UINT32 i = 0; i < smallestIdxFitIdx; i++)
		{
			freeIdxIter++;
			freeIdxChunkIdx = (*freeIdxIter);
		}

		mFreeIdxChunks.erase(freeIdxIter);

		ChunkData& vertChunk = mVertChunks[freeVertChunkIdx];
		ChunkData& idxChunk = mIdxChunks[freeIdxChunkIdx];

		UINT32 vertChunkStart = vertChunk.start;
		UINT32 idxChunkStart = idxChunk.start;

		UINT32 remainingNumVerts = vertChunk.size - meshData->getNumVertices();
		UINT32 remainingNumIdx = idxChunk.size - meshData->getNumIndices();

		vertChunk.size = meshData->getNumVertices();
		idxChunk.size = meshData->getNumIndices();

		if (remainingNumVerts > 0)
		{
			if (!mEmptyVertChunks.empty())
			{
				UINT32 emptyChunkIdx = mEmptyVertChunks.top();
				ChunkData& emptyChunk = mVertChunks[emptyChunkIdx];
				mEmptyVertChunks.pop();

				emptyChunk.start = vertChunkStart + meshData->getNumVertices();
				emptyChunk.size = remainingNumVerts;
			}
			else
			{
				ChunkData newChunk;
				newChunk.size = remainingNumVerts;
				newChunk.start = vertChunkStart + meshData->getNumVertices();

				mVertChunks.push_back(newChunk);
				mFreeVertChunks.push_back((UINT32)(mVertChunks.size() - 1));
			}
		}

		if (remainingNumIdx > 0)
		{
			if (!mEmptyIdxChunks.empty())
			{
				UINT32 emptyChunkIdx = mEmptyIdxChunks.top();
				ChunkData& emptyChunk = mIdxChunks[emptyChunkIdx];
				mEmptyIdxChunks.pop();

				emptyChunk.start = idxChunkStart + meshData->getNumIndices();
				emptyChunk.size = remainingNumIdx;
			}
			else
			{
				ChunkData newChunk;
				newChunk.size = remainingNumIdx;
				newChunk.start = idxChunkStart + meshData->getNumIndices();

				mIdxChunks.push_back(newChunk);
				mFreeIdxChunks.push_back((UINT32)(mIdxChunks.size() - 1));
			}
		}

		AllocatedData newAllocData;
		newAllocData.vertChunkIdx = freeVertChunkIdx;
		newAllocData.idxChunkIdx = freeIdxChunkIdx;
		newAllocData.useFlags = UseFlags::GPUFree;
		newAllocData.eventQueryIdx = createEventQuery();
		newAllocData.mesh = mesh;

		mMeshAllocData[mesh->getMeshHeapId()] = newAllocData;

		// Actually copy data
		for (UINT32 i = 0; i <= mVertexDesc->getMaxStreamIdx(); i++)
		{
			if (!mVertexDesc->hasStream(i))
				continue;

			if (!meshData->getVertexDesc()->hasStream(i))
				continue;

			// Ensure vertex sizes match
			UINT32 vertSize = mVertexData->vertexDeclaration->getProperties().getVertexSize(i);
			UINT32 otherVertSize = meshData->getVertexDesc()->getVertexStride(i);
			if (otherVertSize != vertSize)
			{
				BS_EXCEPT(InvalidParametersException, "Provided vertex size for stream " + toString(i) + " doesn't match meshes vertex size. Needed: " +
					toString(vertSize) + ". Got: " + toString(otherVertSize));
			}

			SPtr<VertexBuffer> vertexBuffer = mVertexData->getBuffer(i);

			UINT8* vertDest = mCPUVertexData[i] + vertChunkStart * vertSize;
			memcpy(vertDest, meshData->getStreamData(i), meshData->getNumVertices() * vertSize);

			vertexBuffer->writeData(vertChunkStart * vertSize, meshData->getNumVertices() * vertSize, vertDest, BTW_NO_OVERWRITE);
		}

		const IndexBufferProperties& ibProps = mIndexBuffer->getProperties();

		UINT32 idxSize = ibProps.getIndexSize();

		// Ensure index sizes match
		if (meshData->getIndexElementSize() != idxSize)
		{
			BS_EXCEPT(InvalidParametersException, "Provided index size doesn't match meshes index size. Needed: " +
				toString(idxSize) + ". Got: " + toString(meshData->getIndexElementSize()));
		}

		UINT8* idxDest = mCPUIndexData + idxChunkStart * idxSize;
		memcpy(idxDest, meshData->getIndexData(), meshData->getNumIndices() * idxSize);
		mIndexBuffer->writeData(idxChunkStart * idxSize, meshData->getNumIndices() * idxSize, idxDest, BTW_NO_OVERWRITE);
	}

	void MeshHeap::dealloc(SPtr<TransientMesh> mesh)
	{
		auto findIter = mMeshAllocData.find(mesh->getMeshHeapId());
		assert(findIter != mMeshAllocData.end());

		AllocatedData& allocData = findIter->second;
		if (allocData.useFlags == UseFlags::GPUFree)
		{
			allocData.useFlags = UseFlags::Free;
			freeEventQuery(allocData.eventQueryIdx);

			mFreeVertChunks.push_back(allocData.vertChunkIdx);
			mFreeIdxChunks.push_back(allocData.idxChunkIdx);

			mergeWithNearbyChunks(allocData.vertChunkIdx, allocData.idxChunkIdx);

			mMeshAllocData.erase(findIter);
		}
		else if (allocData.useFlags == UseFlags::Used)
			allocData.useFlags = UseFlags::CPUFree;
	}

	void MeshHeap::growVertexBuffer(UINT32 numVertices)
	{
		mNumVertices = numVertices;
		mVertexData = SPtr<VertexData>(bs_new<VertexData>());

		mVertexData->vertexCount = mNumVertices;
		mVertexData->vertexDeclaration = VertexDeclaration::create(mVertexDesc, mDeviceMask);

		// Create buffers and copy data
		for (UINT32 i = 0; i <= mVertexDesc->getMaxStreamIdx(); i++)
		{
			if (!mVertexDesc->hasStream(i))
				continue;

			UINT32 vertSize = mVertexData->vertexDeclaration->getProperties().getVertexSize(i);

			VERTEX_BUFFER_DESC desc;
			desc.vertexSize = vertSize;
			desc.numVerts = mVertexData->vertexCount;
			desc.usage = GBU_DYNAMIC;

			SPtr<VertexBuffer> vertexBuffer = VertexBuffer::create(desc, mDeviceMask);
			mVertexData->setBuffer(i, vertexBuffer);

			// Copy all data to the new buffer
			UINT8* oldBuffer = mCPUVertexData[i];
			UINT8* buffer = (UINT8*)bs_alloc(vertSize * numVertices);

			UINT32 destOffset = 0;
			if (oldBuffer != nullptr)
			{
				for (auto& allocData : mMeshAllocData)
				{
					ChunkData& oldChunk = mVertChunks[allocData.second.vertChunkIdx];

					UINT8* oldData = oldBuffer + oldChunk.start * vertSize;
					memcpy(buffer + destOffset * vertSize, oldData, oldChunk.size * vertSize);

					destOffset += oldChunk.size;
				}

				bs_free(oldBuffer);
			}

			if (destOffset > 0)
				vertexBuffer->writeData(0, destOffset * vertSize, buffer, BTW_NO_OVERWRITE);

			mCPUVertexData[i] = buffer;
		}

		// Reorder chunks
		UINT32 destOffset = 0;
		Vector<ChunkData> newVertChunks;
		List<UINT32> freeVertChunks;

		for (auto& allocData : mMeshAllocData)
		{
			ChunkData& oldChunk = mVertChunks[allocData.second.vertChunkIdx];

			ChunkData newChunk;
			newChunk.start = destOffset;
			newChunk.size = oldChunk.size;

			allocData.second.vertChunkIdx = (UINT32)newVertChunks.size();
			newVertChunks.push_back(newChunk);

			destOffset += oldChunk.size;
		}

		// Add free chunk
		if (destOffset != mNumVertices)
		{
			ChunkData newChunk;
			newChunk.start = destOffset;
			newChunk.size = mNumVertices - destOffset;

			newVertChunks.push_back(newChunk);
			freeVertChunks.push_back((UINT32)(newVertChunks.size() - 1));
		}

		mVertChunks = newVertChunks;
		mFreeVertChunks = freeVertChunks;

		while (!mEmptyVertChunks.empty())
			mEmptyVertChunks.pop();
	}

	void MeshHeap::growIndexBuffer(UINT32 numIndices)
	{
		mNumIndices = numIndices;

		INDEX_BUFFER_DESC ibDesc;
		ibDesc.indexType = mIndexType;
		ibDesc.numIndices = mNumIndices;
		ibDesc.usage = GBU_DYNAMIC;

		mIndexBuffer = IndexBuffer::create(ibDesc, mDeviceMask);

		const IndexBufferProperties& ibProps = mIndexBuffer->getProperties();

		// Copy all data to the new buffer
		UINT32 idxSize = ibProps.getIndexSize();

		UINT8* oldBuffer = mCPUIndexData;
		UINT8* buffer = (UINT8*)bs_alloc(idxSize * numIndices);

		UINT32 destOffset = 0;
		if (oldBuffer != nullptr)
		{
			for (auto& allocData : mMeshAllocData)
			{
				ChunkData& oldChunk = mIdxChunks[allocData.second.idxChunkIdx];

				UINT8* oldData = oldBuffer + oldChunk.start * idxSize;
				memcpy(buffer + destOffset * idxSize, oldData, oldChunk.size * idxSize);

				destOffset += oldChunk.size;
			}

			bs_free(oldBuffer);
		}

		if (destOffset > 0)
			mIndexBuffer->writeData(0, destOffset * idxSize, buffer, BTW_NO_OVERWRITE);

		mCPUIndexData = buffer;

		// Reorder chunks
		destOffset = 0;
		Vector<ChunkData> newIdxChunks;
		List<UINT32> freeIdxChunks;

		for (auto& allocData : mMeshAllocData)
		{
			ChunkData& oldChunk = mIdxChunks[allocData.second.idxChunkIdx];

			ChunkData newChunk;
			newChunk.start = destOffset;
			newChunk.size = oldChunk.size;

			allocData.second.idxChunkIdx = (UINT32)newIdxChunks.size();
			newIdxChunks.push_back(newChunk);

			destOffset += oldChunk.size;
		}

		// Add free chunk
		if (destOffset != mNumIndices)
		{
			ChunkData newChunk;
			newChunk.start = destOffset;
			newChunk.size = mNumIndices - destOffset;

			newIdxChunks.push_back(newChunk);
			freeIdxChunks.push_back((UINT32)(newIdxChunks.size() - 1));
		}

		mIdxChunks = newIdxChunks;
		mFreeIdxChunks = freeIdxChunks;

		while (!mEmptyIdxChunks.empty())
			mEmptyIdxChunks.pop();
	}

	UINT32 MeshHeap::createEventQuery()
	{
		UINT32 idx = 0;
		if (mFreeEventQueries.size() > 0)
		{
			idx = mFreeEventQueries.top();
			mFreeEventQueries.pop();
		}
		else
		{
			QueryData newQuery;
			newQuery.query = EventQuery::create();
			newQuery.queryId = 0;

			mEventQueries.push_back(newQuery);
			idx = (UINT32)(mEventQueries.size() - 1);
		}

		return idx;
	}

	void MeshHeap::freeEventQuery(UINT32 idx)
	{
		mEventQueries[idx].query->onTriggered.clear();
		mEventQueries[idx].queryId = 0;
		mFreeEventQueries.push(idx);
	}

	SPtr<VertexData> MeshHeap::getVertexData() const
	{
		return mVertexData;
	}

	SPtr<IndexBuffer> MeshHeap::getIndexBuffer() const
	{
		return mIndexBuffer;
	}

	SPtr<VertexDataDesc> MeshHeap::getVertexDesc() const
	{
		return mVertexDesc;
	}

	UINT32 MeshHeap::getVertexOffset(UINT32 meshId) const
	{
		auto findIter = mMeshAllocData.find(meshId);
		assert(findIter != mMeshAllocData.end());

		UINT32 chunkIdx = findIter->second.vertChunkIdx;
		return mVertChunks[chunkIdx].start;
	}

	UINT32 MeshHeap::getIndexOffset(UINT32 meshId) const
	{
		auto findIter = mMeshAllocData.find(meshId);
		assert(findIter != mMeshAllocData.end());

		UINT32 chunkIdx = findIter->second.idxChunkIdx;
		return mIdxChunks[chunkIdx].start;
	}

	void MeshHeap::notifyUsedOnGPU(UINT32 meshId)
	{
		auto findIter = mMeshAllocData.find(meshId);
		assert(findIter != mMeshAllocData.end());

		AllocatedData& allocData = findIter->second;
		assert(allocData.useFlags != UseFlags::Free);

		if (allocData.useFlags == UseFlags::GPUFree)
			allocData.useFlags = UseFlags::Used;

		SPtr<MeshHeap> thisPtr = std::static_pointer_cast<MeshHeap>(getThisPtr());

		QueryData& queryData = mEventQueries[allocData.eventQueryIdx];
		queryData.queryId = mNextQueryId++;
		queryData.query->onTriggered.clear();
		queryData.query->onTriggered.connect(std::bind(&MeshHeap::queryTriggered, thisPtr, meshId, queryData.queryId));
		queryData.query->begin();
	}

	// Note: Need to use a shared ptr here to ensure MeshHeap doesn't get deallocated sometime during this callback
	void MeshHeap::queryTriggered(SPtr<MeshHeap> thisPtr, UINT32 meshId, UINT32 queryId)
	{
		auto findIter = thisPtr->mMeshAllocData.find(meshId);
		assert(findIter != thisPtr->mMeshAllocData.end());

		AllocatedData& allocData = findIter->second;

		// If query ids don't match then it means there either a more recent query or
		// the buffer was discarded and we are not interested in query result
		QueryData& queryData = thisPtr->mEventQueries[allocData.eventQueryIdx];
		if (queryId == queryData.queryId)
		{
			assert(allocData.useFlags != UseFlags::Free && allocData.useFlags != UseFlags::GPUFree);

			if (allocData.useFlags == UseFlags::CPUFree)
			{
				allocData.useFlags = UseFlags::Free;
				thisPtr->freeEventQuery(allocData.eventQueryIdx);

				thisPtr->mFreeVertChunks.push_back(allocData.vertChunkIdx);
				thisPtr->mFreeIdxChunks.push_back(allocData.idxChunkIdx);

				thisPtr->mergeWithNearbyChunks(allocData.vertChunkIdx, allocData.idxChunkIdx);

				thisPtr->mMeshAllocData.erase(findIter);
			}
			else
				allocData.useFlags = UseFlags::GPUFree;
		}

		queryData.query->onTriggered.clear();
	}

	void MeshHeap::mergeWithNearbyChunks(UINT32 chunkVertIdx, UINT32 chunkIdxIdx)
	{
		// Merge vertex chunks
		ChunkData& vertChunk = mVertChunks[chunkVertIdx];
		for (auto& freeChunkIdx : mFreeVertChunks)
		{
			if (chunkVertIdx == freeChunkIdx)
				continue;

			ChunkData& curChunk = mVertChunks[freeChunkIdx];
			if (curChunk.size == 0) // Already merged
				continue;

			bool merged = false;
			if (curChunk.start == (vertChunk.start + vertChunk.size))
			{
				vertChunk.size += curChunk.size;

				merged = true;
			}
			else if ((curChunk.start + curChunk.size) == vertChunk.start)
			{
				vertChunk.start = curChunk.start;
				vertChunk.size += curChunk.size;

				merged = true;
			}

			if (merged)
			{
				// We can't remove the chunk since that would break the indexing scheme, so
				// mark it as empty and set size to 0. It will be reused when needed.
				curChunk.start = 0;
				curChunk.size = 0;
				mEmptyVertChunks.push(freeChunkIdx);
			}
		}

		// Merge index chunks
		ChunkData& idxChunk = mIdxChunks[chunkIdxIdx];
		for (auto& freeChunkIdx : mFreeIdxChunks)
		{
			if (chunkIdxIdx == freeChunkIdx)
				continue;

			ChunkData& curChunk = mIdxChunks[freeChunkIdx];
			if (curChunk.size == 0) // Already merged
				continue;

			bool merged = false;
			if (curChunk.start == (idxChunk.start + idxChunk.size))
			{
				idxChunk.size += curChunk.size;

				merged = true;
			}
			else if ((curChunk.start + curChunk.size) == idxChunk.start)
			{
				idxChunk.start = curChunk.start;
				idxChunk.size += curChunk.size;

				merged = true;
			}

			if (merged)
			{
				// We can't remove the chunk since that would break the indexing scheme, so
				// mark it as empty and set size to 0. It will be reused when needed.
				curChunk.start = 0;
				curChunk.size = 0;
				mEmptyIdxChunks.push(freeChunkIdx);
			}
		}
	}
	}
}
