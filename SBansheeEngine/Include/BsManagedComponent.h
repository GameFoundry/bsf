#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmComponent.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedComponent : public CM::Component
	{
	public:

	private:
		ScriptComponent* mScriptComponent;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:

		/** Standard constructor.
        */
		ManagedComponent(const CM::HSceneObject& parent);

	public:
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedComponentRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;

	protected:
		ManagedComponent() {} // Serialization only
	};
}