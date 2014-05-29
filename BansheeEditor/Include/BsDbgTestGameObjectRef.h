#pragma once

#include "BsPrerequisites.h"
#include "BsComponent.h"

namespace BansheeEngine
{
	class DbgTestGameObjectRef : public Component
	{
	public:
		HRenderable mRenderable;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		/** Standard constructor.
        */
		DbgTestGameObjectRef(const HSceneObject& parent);

	public:
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class DbgTestGameObjectRefRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		DbgTestGameObjectRef() {} // Serialization only
	};
}