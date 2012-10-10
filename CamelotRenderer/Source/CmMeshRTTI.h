#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmMesh.h"

namespace CamelotEngine
{
	class MeshRTTI : public RTTIType<Mesh, Resource, MeshRTTI>
	{
	public:
		MeshRTTI()
		{

		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<Mesh>(new Mesh());
		}

		virtual const String& getRTTIName() 
		{
			static String name = "Mesh";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return TID_Mesh;
		}
	};
}