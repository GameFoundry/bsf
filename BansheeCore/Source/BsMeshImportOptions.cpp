#include "BsMeshImportOptions.h"
#include "BsMeshImportOptionsRTTI.h"

namespace BansheeEngine
{
	MeshImportOptions::MeshImportOptions()
		:mCPUReadable(false), mImportNormals(true), mImportTangents(true),
		mImportBlendShapes(false), mImportSkin(false), mImportAnimation(false),
		mImportScale(1.0f)
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