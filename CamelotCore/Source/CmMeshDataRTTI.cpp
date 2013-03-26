#include "CmMeshDataRTTI.h"

namespace CamelotEngine
{
	void MeshDataRTTI::onSerializationStarted(IReflectable* obj)
	{
		MeshData* meshData = static_cast<MeshData*>(obj);

		auto tempData = std::make_shared<TempMeshData>();
		for(auto vertElems : meshData->mVertexData)
		{
			for(auto vertElem : vertElems.second)
				tempData->vertexElements.push_back(vertElem);
		}

		meshData->mRTTIData = tempData;
	}

	void MeshDataRTTI::onSerializationEnded(IReflectable* obj)
	{
		MeshData* meshData = static_cast<MeshData*>(obj);
		meshData->mRTTIData = nullptr;
	}
}