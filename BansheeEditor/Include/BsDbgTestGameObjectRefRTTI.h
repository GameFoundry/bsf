#pragma once

#include "BsPrerequisites.h"
#include "CmRTTIType.h"
#include "BsDbgTestGameObjectRef.h"
#include "CmGameObjectRTTI.h"

namespace BansheeEditor
{
	class DbgTestGameObjectRefRTTI : public CM::RTTIType<DbgTestGameObjectRef, CM::Component, DbgTestGameObjectRefRTTI>
	{
	private:
		BS::HRenderable& getRenderable(DbgTestGameObjectRef* obj) { return obj->mRenderable; }
		void setRenderable(DbgTestGameObjectRef* obj, BS::HRenderable& val) 
		{ 
			obj->mRenderable = val; 
		} 

	public:
		DbgTestGameObjectRefRTTI()
		{
			addReflectableField("mRenderable", 0, &DbgTestGameObjectRefRTTI::getRenderable, &DbgTestGameObjectRefRTTI::setRenderable);
		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "DbgTestGameObjectRef";
			return name;
		}

		virtual CM::UINT32 getRTTIId()
		{
			return 600001; // NOTE: Just a debug value
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::GameObjectRTTI::createGameObject<DbgTestGameObjectRef>();
		}
	};
}