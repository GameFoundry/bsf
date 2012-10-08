#include "CmMeshData.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmMeshDataST.h"

namespace CamelotEngine
{
	MeshData::VertexData::VertexData(UINT32 vertexCount)
		:vertex(nullptr), normal(nullptr), tangent(nullptr), 
		bitangent(nullptr), uv0(nullptr), uv1(nullptr)
	{
		this->vertexCount = vertexCount;
	}

	MeshData::VertexData::~VertexData()
	{
		if(vertex != nullptr)
			delete [] vertex;

		if(normal != nullptr)
			delete [] normal;

		if(tangent != nullptr)
			delete [] tangent;

		if(bitangent != nullptr)
			delete [] bitangent;

		if(uv0 != nullptr)
			delete [] uv0;

		if(uv1 != nullptr)
			delete [] uv1;
	}

	RTTITypeBase* MeshData::VertexData::getRTTIStatic()
	{
		return VertexDataST::instance();
	}

	RTTITypeBase* MeshData::VertexData::getRTTI() const
	{
		return getRTTIStatic();
	}

	MeshData::MeshData()
	   :index(nullptr),
		indexCount(0),
		vertexCount(0)
	{ }

	MeshData::~MeshData()
	{
		if(index != nullptr)
			delete [] index;

		vertexBuffers.clear();
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* MeshData::getRTTIStatic()
	{
		return MeshDataST::instance();
	}

	RTTITypeBase* MeshData::getRTTI() const
	{
		return MeshData::getRTTIStatic();
	}
}