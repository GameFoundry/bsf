#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGameObject.h"

namespace CamelotFramework
{
	class CM_EXPORT GameObjectRTTI : public RTTIType<GameObject, IReflectable, GameObjectRTTI>
	{
	private:

	public:
		GameObjectRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "GameObject";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GameObject;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}