#include "BsDefaultMeshData.h"
#include "BsVertexDataDesc.h"
#include "BsVector2.h"
#include "BsVector3.h"
#include "BsVector4.h"
#include "BsColor.h"
#include "BsPixelUtil.h"

namespace BansheeEngine
{
	DefaultMeshData::DefaultMeshData(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType)
	{
		VertexDataDescPtr vertexDesc = vertexLayoutVertexDesc(layout);

		mMeshData = bs_shared_ptr<MeshData>(numVertices, numIndices, vertexDesc, indexType);
	}

	DefaultMeshData::DefaultMeshData(const MeshDataPtr& meshData)
		:mMeshData(meshData)
	{

	}

	void DefaultMeshData::getPositions(Vector3* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_POSITION))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector3) == size);

		mMeshData->getVertexData(VES_POSITION, (UINT8*)buffer, size);
	}

	void DefaultMeshData::setPositions(Vector3* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_POSITION))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector3) == size);

		mMeshData->setVertexData(VES_POSITION, (UINT8*)buffer, size);
	}

	void DefaultMeshData::getNormals(Vector3* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_NORMAL))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector3) == size);

		mMeshData->getVertexData(VES_NORMAL, (UINT8*)buffer, size);
	}

	void DefaultMeshData::setNormals(Vector3* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_NORMAL))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector3) == size);

		mMeshData->setVertexData(VES_NORMAL, (UINT8*)buffer, size);
	}

	void DefaultMeshData::getTangents(Vector4* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_TANGENT))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector4) == size);

		mMeshData->getVertexData(VES_TANGENT, (UINT8*)buffer, size);
	}

	void DefaultMeshData::setTangents(Vector4* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_TANGENT))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector4) == size);

		mMeshData->setVertexData(VES_TANGENT, (UINT8*)buffer, size);
	}

	void DefaultMeshData::getColors(Color* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_COLOR))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector4) == size);

		UINT8* colorSrc = mMeshData->getElementData(VES_COLOR);
		UINT32 stride = mMeshData->getVertexDesc()->getVertexStride(0);

		Color* colorDst = buffer;
		for (UINT32 i = 0; i < numElements; i++)
		{
			PixelUtil::unpackColor(colorDst, PF_R8G8B8A8, (void*)colorSrc);

			colorSrc += stride;
			colorDst++;
		}
	}

	void DefaultMeshData::setColors(Color* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_COLOR))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector4) == size);

		UINT8* colorDst = mMeshData->getElementData(VES_COLOR);
		UINT32 stride = mMeshData->getVertexDesc()->getVertexStride(0);

		Color* colorSrc = buffer;
		for (UINT32 i = 0; i < numElements; i++)
		{
			PixelUtil::packColor(*colorSrc, PF_R8G8B8A8, (void*)colorDst);

			colorSrc++;
			colorDst += stride;
		}
	}

	void DefaultMeshData::getUV0(Vector2* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_TEXCOORD, 0))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector2) == size);

		mMeshData->getVertexData(VES_TEXCOORD, (UINT8*)buffer, size, 0);
	}

	void DefaultMeshData::setUV0(Vector2* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_TEXCOORD, 0))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector2) == size);

		mMeshData->setVertexData(VES_TEXCOORD, (UINT8*)buffer, size, 0);
	}

	void DefaultMeshData::getUV1(Vector2* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_TEXCOORD, 1))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector2) == size);

		mMeshData->getVertexData(VES_TEXCOORD, (UINT8*)buffer, size, 1);
	}

	void DefaultMeshData::setUV1(Vector2* buffer, UINT32 size)
	{
		if (!mMeshData->getVertexDesc()->hasElement(VES_TEXCOORD, 1))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(Vector2) == size);

		mMeshData->setVertexData(VES_TEXCOORD, (UINT8*)buffer, size, 1);
	}

	void DefaultMeshData::getBoneWeights(BoneWeight* buffer, UINT32 size)
	{
		VertexDataDescPtr vertexDesc = mMeshData->getVertexDesc();

		if (!vertexDesc->hasElement(VES_BLEND_WEIGHTS) ||
			!vertexDesc->hasElement(VES_BLEND_INDICES))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(BoneWeight) == size);

		UINT8* weightPtr = mMeshData->getElementData(VES_BLEND_WEIGHTS);
		UINT8* indexPtr = mMeshData->getElementData(VES_BLEND_INDICES);

		UINT32 stride = vertexDesc->getVertexStride(0);

		BoneWeight* weightDst = buffer;
		for (UINT32 i = 0; i < numElements; i++)
		{
			int* indices = (int*)indexPtr;
			float* weights = (float*)weightPtr;

			weightDst->index0 = indices[0];
			weightDst->index1 = indices[1];
			weightDst->index2 = indices[2];
			weightDst->index3 = indices[3];

			weightDst->weight0 = weights[0];
			weightDst->weight1 = weights[1];
			weightDst->weight2 = weights[2];
			weightDst->weight3 = weights[3];

			weightDst++;
			indexPtr += stride;
			weightPtr += stride;
		}
	}

	void DefaultMeshData::setBoneWeights(BoneWeight* buffer, UINT32 size)
	{
		VertexDataDescPtr vertexDesc = mMeshData->getVertexDesc();

		if (!vertexDesc->hasElement(VES_BLEND_WEIGHTS) ||
			!vertexDesc->hasElement(VES_BLEND_INDICES))
			return;

		UINT32 numElements = mMeshData->getNumVertices();
		assert(numElements * sizeof(BoneWeight) == size);

		UINT8* weightPtr = mMeshData->getElementData(VES_BLEND_WEIGHTS);
		UINT8* indexPtr = mMeshData->getElementData(VES_BLEND_INDICES);

		UINT32 stride = vertexDesc->getVertexStride(0);

		BoneWeight* weightSrc = buffer;
		for (UINT32 i = 0; i < numElements; i++)
		{
			int* indices = (int*)indexPtr;
			float* weights = (float*)weightPtr;

			indices[0] = weightSrc->index0;
			indices[1] = weightSrc->index1;
			indices[2] = weightSrc->index2;
			indices[3] = weightSrc->index3;

			weights[0] = weightSrc->weight0;
			weights[1] = weightSrc->weight1;
			weights[2] = weightSrc->weight2;
			weights[3] = weightSrc->weight3;

			weightSrc++;
			indexPtr += stride;
			weightPtr += stride;
		}
	}

	void DefaultMeshData::getIndices(UINT32* buffer, UINT32 size)
	{
		UINT32 indexSize = mMeshData->getIndexElementSize();
		UINT32 numIndices = mMeshData->getNumIndices();

		assert(numIndices * indexSize == size);

		if (mMeshData->getIndexType() == IT_16BIT)
		{
			UINT16* src = mMeshData->getIndices16();
			UINT32* dest = buffer;
			
			for (UINT32 i = 0; i < numIndices; i++)
			{
				*dest = *src;

				src++;
				dest++;
			}
		}
		else
		{
			memcpy(buffer, mMeshData->getIndices32(), size);
		}
	}

	void DefaultMeshData::setIndices(UINT32* buffer, UINT32 size)
	{
		UINT32 indexSize = mMeshData->getIndexElementSize();
		UINT32 numIndices = mMeshData->getNumIndices();

		assert(numIndices * indexSize == size);

		if (mMeshData->getIndexType() == IT_16BIT)
		{
			UINT16* dest = mMeshData->getIndices16();
			UINT32* src = buffer;

			for (UINT32 i = 0; i < numIndices; i++)
			{
				*dest = *src;

				src++;
				dest++;
			}
		}
		else
		{
			memcpy(mMeshData->getIndices32(), buffer, size);
		}
	}

	DefaultMeshDataPtr DefaultMeshData::create(UINT32 numVertices, UINT32 numIndices, VertexLayout layout, IndexType indexType)
	{
		return bs_shared_ptr<DefaultMeshData>(numVertices, numIndices, layout, indexType);
	}

	DefaultMeshDataPtr DefaultMeshData::create(const MeshDataPtr& meshData)
	{
		return bs_shared_ptr<DefaultMeshData>(meshData);
	}

	VertexDataDescPtr DefaultMeshData::vertexLayoutVertexDesc(VertexLayout type)
	{
		VertexDataDescPtr vertexDesc = VertexDataDesc::create();

		INT32 intType = (INT32)type;

		if (intType == 0)
			type = VertexLayout::Position;

		if ((intType & (INT32)VertexLayout::Position) != 0)
			vertexDesc->addVertElem(VET_FLOAT3, VES_POSITION);

		if ((intType & (INT32)VertexLayout::Normal) != 0)
			vertexDesc->addVertElem(VET_FLOAT3, VES_NORMAL);

		if ((intType & (INT32)VertexLayout::Tangent) != 0)
			vertexDesc->addVertElem(VET_FLOAT4, VES_TANGENT);

		if ((intType & (INT32)VertexLayout::UV0) != 0)
			vertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD, 0);

		if ((intType & (INT32)VertexLayout::UV1) != 0)
			vertexDesc->addVertElem(VET_FLOAT2, VES_TEXCOORD, 1);

		if ((intType & (INT32)VertexLayout::Color) != 0)
			vertexDesc->addVertElem(VET_FLOAT4, VES_COLOR);

		if ((intType & (INT32)VertexLayout::BoneWeights) != 0)
		{
			vertexDesc->addVertElem(VET_UBYTE4, VES_BLEND_INDICES);
			vertexDesc->addVertElem(VET_FLOAT4, VES_BLEND_WEIGHTS);
		}

		return vertexDesc;
	}
}