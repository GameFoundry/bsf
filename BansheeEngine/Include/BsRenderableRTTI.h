#pragma once

#include "BsPrerequisites.h"
#include "CmRTTIType.h"
#include "BsRenderable.h"

namespace BansheeEngine
{
	class BS_EXPORT RenderableRTTI : public CM::RTTIType<Renderable, CM::Component, RenderableRTTI>
	{
	private:

	public:
		RenderableRTTI()
		{

		}

		virtual const CM::String& getRTTIName()
		{
			static CM::String name = "Renderable";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_Renderable;
		}

		virtual std::shared_ptr<CM::IReflectable> newRTTIObject()
		{
			return CM::cm_shared_ptr<Renderable, CM::PoolAlloc>(new (CM::cm_alloc<Renderable, CM::PoolAlloc>()) Renderable());
		}
	};
}