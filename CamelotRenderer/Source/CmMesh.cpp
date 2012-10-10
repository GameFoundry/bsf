#include "CmMesh.h"
#include "CmMeshRTTI.h"

namespace CamelotEngine
{
	Mesh::Mesh()
	{

	}

	Mesh::~Mesh()
	{
	}

	void Mesh::prepare(MeshDataPtr meshData)
	{
		mMeshData = meshData;
	}

	void Mesh::load()
	{
		if(mMeshData == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Cannot load mesh. Mesh data hasn't been set.");
		}

		throw std::exception("The method or operation is not implemented.");
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