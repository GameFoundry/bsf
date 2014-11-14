#include "BsMesh.h"
#include "BsMeshRTTI.h"
#include "BsMeshData.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsDebug.h"
#include "BsHardwareBufferManager.h"
#include "BsMeshManager.h"
#include "BsCoreThread.h"
#include "BsAsyncOp.h"
#include "BsAABox.h"
#include "BsVertexDataDesc.h"
#include "BsResources.h"
#include "BsFrameAlloc.h"

namespace BansheeEngine
{
	MeshCore::MeshCore(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc,
		const Vector<SubMesh>& subMeshes, MeshBufferType bufferType, IndexType indexType, MeshDataPtr initialMeshData)
		:MeshCoreBase(numVertices, numIndices, subMeshes), mVertexData(nullptr), mIndexBuffer(nullptr), 
		mVertexDesc(vertexDesc), mBufferType(bufferType), mIndexType(indexType), mTempInitialMeshData(initialMeshData)
	{ }

	void MeshCore::initialize()
	{
		THROW_IF_NOT_CORE_THREAD;

		mIndexBuffer = HardwareBufferCoreManager::instance().createIndexBuffer(mIndexType,
			mProperties.mNumIndices, mBufferType == MeshBufferType::Dynamic ? GBU_DYNAMIC : GBU_STATIC);

		mVertexData = std::shared_ptr<VertexData>(bs_new<VertexData, PoolAlloc>());

		mVertexData->vertexCount = mProperties.mNumVertices;

		List<VertexElement> elements = mVertexDesc->createElements();
		mVertexData->vertexDeclaration = HardwareBufferCoreManager::instance().createVertexDeclaration(elements);

		for (UINT32 i = 0; i <= mVertexDesc->getMaxStreamIdx(); i++)
		{
			if (!mVertexDesc->hasStream(i))
				continue;

			SPtr<VertexBufferCore> vertexBuffer = HardwareBufferCoreManager::instance().createVertexBuffer(
				mVertexData->vertexDeclaration->getProperties().getVertexSize(i),
				mVertexData->vertexCount,
				mBufferType == MeshBufferType::Dynamic ? GBU_DYNAMIC : GBU_STATIC);

			mVertexData->setBuffer(i, vertexBuffer);
		}

		// TODO Low priority - DX11 (and maybe OpenGL)? allow an optimization that allows you to set
		// buffer data upon buffer construction, instead of setting it in a second step like I do here
		if (mTempInitialMeshData != nullptr)
		{
			writeSubresource(0, *mTempInitialMeshData, mBufferType == MeshBufferType::Dynamic);
			mTempInitialMeshData = nullptr;
		}

		MeshCoreBase::initialize();
	}

	void MeshCore::destroy()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mVertexData != nullptr)
		{
			for (UINT32 i = 0; i < mVertexData->getBufferCount(); i++)
			{
				if (mVertexData->getBuffer(i) != nullptr)
					mVertexData->getBuffer(i)->destroy();
			}
		}

		if (mIndexBuffer != nullptr)
			mIndexBuffer->destroy();

		mVertexData = nullptr;
		mIndexBuffer = nullptr;
		mVertexDesc = nullptr;
		mTempInitialMeshData = nullptr;

		MeshCoreBase::destroy();
	}

	std::shared_ptr<VertexData> MeshCore::getVertexData() const
	{
		THROW_IF_NOT_CORE_THREAD;

		return mVertexData;
	}

	SPtr<IndexBufferCore> MeshCore::getIndexBuffer() const
	{
		THROW_IF_NOT_CORE_THREAD;

		return mIndexBuffer;
	}

	void MeshCore::writeSubresource(UINT32 subresourceIdx, const MeshData& meshData, bool discardEntireBuffer, bool performUpdateBounds)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (discardEntireBuffer)
		{
			if (mBufferType == MeshBufferType::Static)
			{
				LOGWRN("Buffer discard is enabled but buffer was not created as dynamic. Disabling discard.");
				discardEntireBuffer = false;
			}
		}
		else
		{
			if (mBufferType == MeshBufferType::Dynamic)
			{
				LOGWRN("Buffer discard is not enabled but buffer was created as dynamic. Enabling discard.");
				discardEntireBuffer = true;
			}
		}

		// Indices
		const IndexBufferProperties& ibProps = mIndexBuffer->getProperties();

		UINT32 indicesSize = meshData.getIndexBufferSize();
		UINT8* srcIdxData = meshData.getIndexData();

		if (meshData.getIndexElementSize() != ibProps.getIndexSize())
		{
			BS_EXCEPT(InvalidParametersException, "Provided index size doesn't match meshes index size. Needed: " +
				toString(ibProps.getIndexSize()) + ". Got: " + toString(meshData.getIndexElementSize()));
		}

		if (indicesSize > mIndexBuffer->getSizeInBytes())
			BS_EXCEPT(InvalidParametersException, "Index buffer values are being written out of valid range.");

		mIndexBuffer->writeData(0, indicesSize, srcIdxData, discardEntireBuffer ? BufferWriteType::Discard : BufferWriteType::Normal);

		// Vertices
		for (UINT32 i = 0; i <= mVertexDesc->getMaxStreamIdx(); i++)
		{
			if (!mVertexDesc->hasStream(i))
				continue;

			if (!meshData.getVertexDesc()->hasStream(i))
				continue;

			// Ensure both have the same sized vertices
			UINT32 myVertSize = mVertexDesc->getVertexStride(i);
			UINT32 otherVertSize = meshData.getVertexDesc()->getVertexStride(i);
			if (myVertSize != otherVertSize)
			{
				BS_EXCEPT(InvalidParametersException, "Provided vertex size for stream " + toString(i) + " doesn't match meshes vertex size. Needed: " +
					toString(myVertSize) + ". Got: " + toString(otherVertSize));
			}

			SPtr<VertexBufferCore> vertexBuffer = mVertexData->getBuffer(i);
			const VertexBufferProperties& vbProps = vertexBuffer->getProperties();

			UINT32 bufferSize = meshData.getStreamSize(i);
			UINT8* srcVertBufferData = meshData.getStreamData(i);

			if (bufferSize > vertexBuffer->getSizeInBytes())
				BS_EXCEPT(InvalidParametersException, "Vertex buffer values for stream \"" + toString(i) + "\" are being written out of valid range.");

			if (RenderSystem::instance().getVertexColorFlipRequired())
			{
				UINT8* bufferCopy = (UINT8*)bs_alloc(bufferSize);
				memcpy(bufferCopy, srcVertBufferData, bufferSize); // TODO Low priority - Attempt to avoid this copy

				UINT32 vertexStride = meshData.getVertexDesc()->getVertexStride(i);
				for (INT32 semanticIdx = 0; semanticIdx < VertexBuffer::MAX_SEMANTIC_IDX; semanticIdx++)
				{
					if (!meshData.getVertexDesc()->hasElement(VES_COLOR, semanticIdx, i))
						continue;

					UINT8* colorData = bufferCopy + mVertexDesc->getElementOffsetFromStream(VES_COLOR, semanticIdx, i);
					for (UINT32 j = 0; j < mVertexData->vertexCount; j++)
					{
						UINT32* curColor = (UINT32*)colorData;

						(*curColor) = ((*curColor) & 0xFF00FF00) | ((*curColor >> 16) & 0x000000FF) | ((*curColor << 16) & 0x00FF0000);

						colorData += vertexStride;
					}
				}

				vertexBuffer->writeData(0, bufferSize, bufferCopy, discardEntireBuffer ? BufferWriteType::Discard : BufferWriteType::Normal);

				bs_free(bufferCopy);
			}
			else
			{
				vertexBuffer->writeData(0, bufferSize, srcVertBufferData, discardEntireBuffer ? BufferWriteType::Discard : BufferWriteType::Normal);
			}
		}

		if (performUpdateBounds)
			updateBounds(meshData);
	}

	void MeshCore::readSubresource(UINT32 subresourceIdx, MeshData& meshData)
	{
		THROW_IF_NOT_CORE_THREAD;

		IndexType indexType = IT_32BIT;
		if (mIndexBuffer)
			indexType = mIndexBuffer->getProperties().getType();

		if (mIndexBuffer)
		{
			const IndexBufferProperties& ibProps = mIndexBuffer->getProperties();

			if (meshData.getIndexElementSize() != ibProps.getIndexSize())
			{
				BS_EXCEPT(InvalidParametersException, "Provided index size doesn't match meshes index size. Needed: " +
					toString(ibProps.getIndexSize()) + ". Got: " + toString(meshData.getIndexElementSize()));
			}

			UINT8* idxData = static_cast<UINT8*>(mIndexBuffer->lock(GBL_READ_ONLY));
			UINT32 idxElemSize = ibProps.getIndexSize();

			UINT8* indices = nullptr;

			if (indexType == IT_16BIT)
				indices = (UINT8*)meshData.getIndices16();
			else
				indices = (UINT8*)meshData.getIndices32();

			UINT32 numIndicesToCopy = std::min(mProperties.mNumIndices, meshData.getNumIndices());

			UINT32 indicesSize = numIndicesToCopy * idxElemSize;
			if (indicesSize > meshData.getIndexBufferSize())
				BS_EXCEPT(InvalidParametersException, "Provided buffer doesn't have enough space to store mesh indices.");

			memcpy(indices, idxData, numIndicesToCopy * idxElemSize);

			mIndexBuffer->unlock();
		}

		if (mVertexData)
		{
			auto vertexBuffers = mVertexData->getBuffers();

			UINT32 streamIdx = 0;
			for (auto iter = vertexBuffers.begin(); iter != vertexBuffers.end(); ++iter)
			{
				if (!meshData.getVertexDesc()->hasStream(streamIdx))
					continue;

				SPtr<VertexBufferCore> vertexBuffer = iter->second;
				const VertexBufferProperties& vbProps = vertexBuffer->getProperties();

				// Ensure both have the same sized vertices
				UINT32 myVertSize = mVertexDesc->getVertexStride(streamIdx);
				UINT32 otherVertSize = meshData.getVertexDesc()->getVertexStride(streamIdx);
				if (myVertSize != otherVertSize)
				{
					BS_EXCEPT(InvalidParametersException, "Provided vertex size for stream " + toString(streamIdx) + " doesn't match meshes vertex size. Needed: " +
						toString(myVertSize) + ". Got: " + toString(otherVertSize));
				}

				UINT32 numVerticesToCopy = meshData.getNumVertices();
				UINT32 bufferSize = vbProps.getVertexSize() * numVerticesToCopy;

				if (bufferSize > vertexBuffer->getSizeInBytes())
					BS_EXCEPT(InvalidParametersException, "Vertex buffer values for stream \"" + toString(streamIdx) + "\" are being read out of valid range.");

				UINT8* vertDataPtr = static_cast<UINT8*>(vertexBuffer->lock(GBL_READ_ONLY));

				UINT8* dest = meshData.getStreamData(streamIdx);
				memcpy(dest, vertDataPtr, bufferSize);

				vertexBuffer->unlock();

				streamIdx++;
			}
		}
	}

	void MeshCore::updateBounds(const MeshData& meshData)
	{
		mProperties.mBounds = meshData.calculateBounds();
		markCoreDirty();
	}

	Mesh::Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		MeshBufferType bufferType, DrawOperationType drawOp, IndexType indexType)
		:MeshBase(numVertices, numIndices, drawOp), mVertexDesc(vertexDesc), mBufferType(bufferType), 
		mIndexType(indexType)
	{

	}

	Mesh::Mesh(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc,
		const Vector<SubMesh>& subMeshes, MeshBufferType bufferType, IndexType indexType)
		:MeshBase(numVertices, numIndices, subMeshes), mVertexDesc(vertexDesc), mBufferType(bufferType), 
		mIndexType(indexType)
	{

	}

	Mesh::Mesh(const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp)
		:MeshBase(initialMeshData->getNumVertices(), initialMeshData->getNumIndices(), drawOp), 
		mIndexType(initialMeshData->getIndexType()), mVertexDesc(initialMeshData->getVertexDesc()), 
		mTempInitialMeshData(initialMeshData)
	{

	}

	Mesh::Mesh(const MeshDataPtr& initialMeshData, const Vector<SubMesh>& subMeshes, MeshBufferType bufferType)
		:MeshBase(initialMeshData->getNumVertices(), initialMeshData->getNumIndices(), subMeshes),
		mIndexType(initialMeshData->getIndexType()), mVertexDesc(initialMeshData->getVertexDesc()), 
		mTempInitialMeshData(initialMeshData)
	{

	}

	Mesh::Mesh()
		:MeshBase(0, 0, DOT_TRIANGLE_LIST), mBufferType(MeshBufferType::Static), mIndexType(IT_32BIT)
	{

	}

	Mesh::~Mesh()
	{
		
	}

	AsyncOp Mesh::writeSubresource(CoreAccessor& accessor, UINT32 subresourceIdx, const MeshDataPtr& data, bool discardEntireBuffer)
	{
		updateBounds(*data);

		data->_lock();

		std::function<void(const SPtr<MeshCore>&, UINT32, const MeshDataPtr&, bool, AsyncOp&)> func =
			[&](const SPtr<MeshCore>& mesh, UINT32 _subresourceIdx, const MeshDataPtr& _meshData, bool _discardEntireBuffer, AsyncOp& asyncOp)
		{
			mesh->writeSubresource(_subresourceIdx, *_meshData, _discardEntireBuffer, false);
			_meshData->_unlock();
			asyncOp._completeOperation();

		};

		return accessor.queueReturnCommand(std::bind(func, getCore(), subresourceIdx,
			 data, discardEntireBuffer, std::placeholders::_1));
	}

	AsyncOp Mesh::readSubresource(CoreAccessor& accessor, UINT32 subresourceIdx, const MeshDataPtr& data)
	{
		data->_lock();

		std::function<void(const SPtr<MeshCore>&, UINT32, const MeshDataPtr&, AsyncOp&)> func =
			[&](const SPtr<MeshCore>& mesh, UINT32 _subresourceIdx, const MeshDataPtr& _meshData, AsyncOp& asyncOp)
		{
			mesh->readSubresource(_subresourceIdx, *_meshData);
			_meshData->_unlock();
			asyncOp._completeOperation();

		};

		return accessor.queueReturnCommand(std::bind(func, getCore(), subresourceIdx,
			data, std::placeholders::_1));
	}

	MeshDataPtr Mesh::allocateSubresourceBuffer(UINT32 subresourceIdx) const
	{
		MeshDataPtr meshData = bs_shared_ptr<MeshData>(mProperties.mNumVertices, mProperties.mNumIndices, mVertexDesc, mIndexType);

		return meshData;
	}

	void Mesh::initialize()
	{
		if (mTempInitialMeshData != nullptr)
		{
			updateBounds(*mTempInitialMeshData);
		}

		MeshBase::initialize();
	}

	void Mesh::updateBounds(const MeshData& meshData)
	{
		mProperties.mBounds = meshData.calculateBounds();
		markCoreDirty();
	}

	SPtr<MeshCore> Mesh::getCore() const
	{
		return std::static_pointer_cast<MeshCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> Mesh::createCore() const
	{
		MeshCore* obj = new (bs_alloc<MeshCore>()) MeshCore(mProperties.mNumVertices, mProperties.mNumIndices, 
			mVertexDesc, mProperties.mSubMeshes, mBufferType, mIndexType, mTempInitialMeshData);

		SPtr<CoreObjectCore> meshCore = bs_shared_ptr<MeshCore, GenAlloc>(obj);
		meshCore->_setThisPtr(meshCore);

		mTempInitialMeshData = nullptr;

		return meshCore;
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
		MeshBufferType bufferType, DrawOperationType drawOp, IndexType indexType)
	{
		MeshPtr meshPtr = _createPtr(numVertices, numIndices, vertexDesc, bufferType, drawOp, indexType);

		return static_resource_cast<Mesh>(gResources()._createResourceHandle(meshPtr));
	}

	HMesh Mesh::create(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc,
		const Vector<SubMesh>& subMeshes, MeshBufferType bufferType, IndexType indexType)
	{
		MeshPtr meshPtr = _createPtr(numVertices, numIndices, vertexDesc, subMeshes, bufferType, indexType);

		return static_resource_cast<Mesh>(gResources()._createResourceHandle(meshPtr));
	}

	HMesh Mesh::create(const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp)
	{
		MeshPtr meshPtr = _createPtr(initialMeshData, bufferType, drawOp);

		return static_resource_cast<Mesh>(gResources()._createResourceHandle(meshPtr));
	}

	HMesh Mesh::create(const MeshDataPtr& initialMeshData, const Vector<SubMesh>& subMeshes, MeshBufferType bufferType)
	{
		MeshPtr meshPtr = _createPtr(initialMeshData, subMeshes, bufferType);

		return static_resource_cast<Mesh>(gResources()._createResourceHandle(meshPtr));
	}

	MeshPtr Mesh::_createPtr(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc, 
		MeshBufferType bufferType, DrawOperationType drawOp, IndexType indexType)
	{
		return MeshManager::instance().create(numVertices, numIndices, vertexDesc, bufferType, drawOp, indexType);
	}

	MeshPtr Mesh::_createPtr(UINT32 numVertices, UINT32 numIndices, const VertexDataDescPtr& vertexDesc,
		const Vector<SubMesh>& subMeshes, MeshBufferType bufferType, IndexType indexType)
	{
		return MeshManager::instance().create(numVertices, numIndices, vertexDesc, subMeshes, bufferType, indexType);
	}

	MeshPtr Mesh::_createPtr(const MeshDataPtr& initialMeshData, MeshBufferType bufferType, DrawOperationType drawOp)
	{
		return MeshManager::instance().create(initialMeshData, bufferType, drawOp);
	}

	MeshPtr Mesh::_createPtr(const MeshDataPtr& initialMeshData, const Vector<SubMesh>& subMeshes, MeshBufferType bufferType)
	{
		return MeshManager::instance().create(initialMeshData, subMeshes, bufferType);
	}
}