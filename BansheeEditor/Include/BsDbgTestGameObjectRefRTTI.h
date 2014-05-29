#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsDbgTestGameObjectRef.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class DbgTestGameObjectRefRTTI : public RTTIType<DbgTestGameObjectRef, Component, DbgTestGameObjectRefRTTI>
	{
	private:
		HRenderable& getRenderable(DbgTestGameObjectRef* obj) { return obj->mRenderable; }
		void setRenderable(DbgTestGameObjectRef* obj, HRenderable& val) 
		{ 
			obj->mRenderable = val; 
		} 

	public:
		DbgTestGameObjectRefRTTI()
		{
			addReflectableField("mRenderable", 0, &DbgTestGameObjectRefRTTI::getRenderable, &DbgTestGameObjectRefRTTI::setRenderable);
		}

		virtual const String& getRTTIName()
		{
			static String name = "DbgTestGameObjectRef";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return 600001; // NOTE: Just a debug value
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GameObjectRTTI::createGameObject<DbgTestGameObjectRef>();
		}
	};
}