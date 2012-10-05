#include "CmMeshData.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmMeshDataST.h"

namespace CamelotEngine
{
	MeshData::MeshData()
	   :vertex(nullptr),
		normal(nullptr),
		tangent(nullptr),
		bitangent(nullptr),
		uv0(nullptr),
		uv1(nullptr),
		index(nullptr),
		indexCount(0),
		vertexCount(0)
	{ }

	MeshData::~MeshData()
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

		if(index != nullptr)
			delete [] index;
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