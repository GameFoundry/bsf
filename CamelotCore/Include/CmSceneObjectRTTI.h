#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmSceneObject.h"

namespace CamelotFramework
{
	class CM_EXPORT SceneObjectRTTI : public RTTIType<SceneObject, GameObject, SceneObjectRTTI>
	{
	private:
		String& getName(SceneObject* obj) { return obj->mName; }
		void setName(SceneObject* obj, String& name) { obj->mName = name; }

	public:
		SceneObjectRTTI()
		{
			addPlainField("mName", 0, &SceneObjectRTTI::getName, &SceneObjectRTTI::setName);
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