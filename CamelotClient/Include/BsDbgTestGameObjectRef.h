#pragma once

#include "BsPrerequisites.h"
#include "CmComponent.h"

namespace BansheeEditor
{
	class DbgTestGameObjectRef : public CM::Component
	{
	public:
		BS::HRenderable mRenderable;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class CM::SceneObject;

		/** Standard constructor.
        */
		DbgTestGameObjectRef(const CM::HSceneObject& parent);

	public:
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class DbgTestGameObjectRefRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;

	protected:
		DbgTestGameObjectRef() {} // Serialization only
	};
}