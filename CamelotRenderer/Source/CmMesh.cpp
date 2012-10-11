#include "CmMesh.h"
#include "CmMeshRTTI.h"
#include "CmHardwareBufferManager.h"

namespace CamelotEngine
{
	Mesh::Mesh()
		:mVertexData(nullptr), mIndexData(nullptr)
	{

	}

	Mesh::~Mesh()
	{
		if(mVertexData)
			delete mVertexData;

		if(mIndexData)
			delete mIndexData;
	}

	void Mesh::prepare(MeshDataPtr meshData)
	{
		mMeshData = meshData;
	}

	void Mesh::loadImpl()
	{
		if(mMeshData == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Cannot load mesh. Mesh data hasn't been set.");
		}

		// Submeshes
		for(int i = 0; i < mMeshData->subMeshes.size(); i++)
			mSubMeshes.push_back(SubMesh(mMeshData->subMeshes[i].indexOffset, mMeshData->subMeshes[i].indexCount));

		// Indices
		mIndexData = new IndexData();

		mIndexData->indexCount = mMeshData->indexCount;
		mIndexData->indexBuffer = HardwareBufferManager::instance().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT,
			mIndexData->indexCount, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		UINT16* idxData = static_cast<UINT16*>(mIndexData->indexBuffer->lock(HardwareBuffer::HBL_NORMAL));

		for(int i = 0; i < mIndexData->indexCount; i++)
		{
			idxData[i] = (UINT16)mMeshData->index[i];
		}

		mIndexData->indexBuffer->unlock();

		// Vertices
		mVertexData = new VertexData();

		mVertexData->vertexStart = 0;
		mVertexData->vertexCount = mMeshData->vertexCount;

		
		//mVertexData->vertexDeclaration = mMeshData->declaration.clone();
		//VertexDeclaration* decl = mVertexData->vertexDeclaration;
		//decl->removeAllElements();

		//size_t offset = 0;
		//decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
		//offset += VertexElement::getTypeSize(VET_FLOAT3);
		//decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		//offset += VertexElement::getTypeSize(VET_FLOAT2);

		//decl->addElement(0, offset, VET_COLOUR, VES_DIFFUSE);
		//offset += VertexElement::getTypeSize(VET_COLOUR);

		//HardwareVertexBufferPtr vertexBuffer = HardwareBufferManager::instance().createVertexBuffer(
		//	mVertexData->vertexDeclaration->getVertexSize(0),
		//	mVertexData->vertexCount,
		//	HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		//mVertexData->vertexBufferBinding->setBinding(0, vertexBuffer);

		//size_t vertexSize = vertexBuffer->getVertexSize();
		//char* vertBufferData = static_cast<char*>(vertexBuffer->lock(HardwareBuffer::HBL_NORMAL));

		//size_t posSize = VertexElement::getTypeSize(VET_FLOAT3);
		//size_t uvSize = VertexElement::getTypeSize(VET_FLOAT2);

		//Vector3 position(-5.0f, -5.0f, -5.0f);
		//memcpy(vertBufferData, &position, posSize);
		//vertBufferData += posSize;

		//Vector2 uv(0.0f, 0.0f);
		//memcpy(vertBufferData, &uv, uvSize);
		//vertBufferData += uvSize;


		//position = 	Vector3(-5.0f, 5.0f, -5.0f);
		//memcpy(vertBufferData, &position, posSize);
		//vertBufferData += posSize;

		//uv = Vector2(0.0f, 1.0f);
		//memcpy(vertBufferData, &uv, uvSize);
		//vertBufferData += uvSize;


		//position = 	Vector3(5.0f, 5.0f, -5.0f);
		//memcpy(vertBufferData, &position, posSize);
		//vertBufferData += posSize;

		//uv = Vector2(1.0f, 1.0f);
		//memcpy(vertBufferData, &uv, uvSize);
		//vertBufferData += uvSize;


		//position = 	Vector3(5.0f, -5.0f, -5.0f);
		//memcpy(vertBufferData, &position, posSize);
		//vertBufferData += posSize;

		//uv = Vector2(1.0f, 0.0f);
		//memcpy(vertBufferData, &uv, uvSize);
		//vertBufferData += uvSize;


		//position = 	Vector3(-5.0f, -5.0f, 5.0f);
		//memcpy(vertBufferData, &position, posSize);
		//vertBufferData += posSize;

		//uv = Vector2(0.0f, 0.0f);
		//memcpy(vertBufferData, &uv, uvSize);
		//vertBufferData += uvSize;


		//position = 	Vector3(5.0f, -5.0f, 5.0f);
		//memcpy(vertBufferData, &position, posSize);
		//vertBufferData += posSize;

		//uv = Vector2(1.0f, 0.0f);
		//memcpy(vertBufferData, &uv, uvSize);
		//vertBufferData += uvSize;


		//position = 	Vector3(5.0f, 5.0f, 5.0f);
		//memcpy(vertBufferData, &position, posSize);
		//vertBufferData += posSize;

		//uv = Vector2(1.0f, 1.0f);
		//memcpy(vertBufferData, &uv, uvSize);
		//vertBufferData += uvSize;


		//position = 	Vector3(-5.0f, 5.0f, 5.0f);
		//memcpy(vertBufferData, &position, posSize);
		//vertBufferData += posSize;

		//uv = Vector2(0.0f, 1.0f);
		//memcpy(vertBufferData, &uv, uvSize);
		//vertBufferData += uvSize;

		//vertexBuffer->unlock();

		mMeshData = nullptr;
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
}