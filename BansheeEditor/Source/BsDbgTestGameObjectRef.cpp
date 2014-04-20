#include "BsDbgTestGameObjectRef.h"
#include "BsDbgTestGameObjectRefRTTI.h"
#include "CmSceneObject.h"
#include "BsRenderable.h"

namespace BansheeEngine
{
	DbgTestGameObjectRef::DbgTestGameObjectRef(const HSceneObject& parent)
		:Component(parent)
	{

	}

	RTTITypeBase* DbgTestGameObjectRef::getRTTIStatic()
	{
		return DbgTestGameObjectRefRTTI::instance();
	}

	RTTITypeBase* DbgTestGameObjectRef::getRTTI() const
	{
		return DbgTestGameObjectRef::getRTTIStatic();
	}
}