#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmRenderable.h"

namespace CamelotEngine
{
	class CM_EXPORT RenderableRTTI : public RTTIType<Renderable, Component, RenderableRTTI>
	{
	private:

	public:
		RenderableRTTI()
		{

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
			return std::shared_ptr<Renderable>(new Renderable());
		}
	};
}