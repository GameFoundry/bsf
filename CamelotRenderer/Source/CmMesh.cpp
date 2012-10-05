#include "CmMesh.h"
#include "CmMeshST.h"

namespace CamelotEngine
{
	void Mesh::load()
	{
		throw std::exception("The method or operation is not implemented.");
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* Mesh::getRTTIStatic()
	{
		return MeshST::instance();
	}

	RTTITypeBase* Mesh::getRTTI() const
	{
		return Mesh::getRTTIStatic();
	}
}