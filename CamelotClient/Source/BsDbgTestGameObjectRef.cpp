#include "BsDbgTestGameObjectRef.h"
#include "BsDbgTestGameObjectRefRTTI.h"
#include "CmSceneObject.h"
#include "BsRenderable.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
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