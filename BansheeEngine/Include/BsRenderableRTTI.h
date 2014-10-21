#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsRenderable.h"
#include "BsGameObjectRTTI.h"

namespace BansheeEngine
{
	class BS_EXPORT RenderableRTTI : public RTTIType<Renderable, Component, RenderableRTTI>
	{
	private:
		RenderableHandlerPtr getInternal(Renderable* obj) { return obj->mInternal; }
		void setInternal(Renderable* obj, RenderableHandlerPtr val) { obj->mInternal = val; }

	public:
		RenderableRTTI()
		{
			addReflectablePtrField("mInternal", 0, &RenderableRTTI::getInternal, &RenderableRTTI::setInternal);
		}

		virtual const String& getRTTIName()
		{
			static String name = "Renderable";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Renderable;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GameObjectRTTI::createGameObject<Renderable>();
		}
	};
}