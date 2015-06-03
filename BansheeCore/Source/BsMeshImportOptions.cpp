#include "BsMeshImportOptions.h"
#include "BsMeshImportOptionsRTTI.h"

namespace BansheeEngine
{
	MeshImportOptions::MeshImportOptions()
		:mCPUReadable(false), mImportNormals(false), mImportTangents(false), 
		mImportBlendShapes(false), mImportSkin(false), mImportAnimation(false),
		mImportScale(0.01f)
	{ }

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* MeshImportOptions::getRTTIStatic()
	{
		return MeshImportOptionsRTTI::instance();
	}

	RTTITypeBase* MeshImportOptions::getRTTI() const
	{
		return MeshImportOptions::getRTTIStatic();
	}
}