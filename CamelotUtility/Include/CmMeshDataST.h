#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmRTTIType.h"
#include "CmMeshData.h"

namespace CamelotEngine
{
	class MeshDataST : public RTTIType<MeshData, IReflectable, MeshDataST>
	{
	public:
		MeshDataST()
		{

		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() 
		{
			return std::shared_ptr<MeshData>(new MeshData());
		}

		virtual const String& getRTTIName() 
		{
			static String name = "MeshData";
			throw name;
		}

		virtual UINT32 getRTTIId() 
		{
			return 103;
		}
	};
}