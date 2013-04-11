#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmSceneObject.h"

namespace CamelotEngine
{
	class CM_EXPORT SceneObjectRTTI : public RTTIType<SceneObject, GameObject, SceneObjectRTTI>
	{
	private:

	public:
		SceneObjectRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "SceneObject";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_SceneObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(NotImplementedException, "This RTTI class is unfinished.");
		}
	};
}