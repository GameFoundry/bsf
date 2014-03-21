#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "CmComponent.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedComponent : public CM::Component
	{
	public:
		MonoObject* getManagedInstance() const { return mManagedInstance; }
		const CM::String& getManagedFullTypeName() const { return mFullTypeName; }

	private:
		MonoObject* mManagedInstance;
		uint32_t mManagedHandle;

		CM::String mNamespace;
		CM::String mTypeName;
		CM::String mFullTypeName;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class CM::SceneObject;

		/** Standard constructor.
        */
		ManagedComponent(const CM::HSceneObject& parent, const CM::String& ns, const CM::String& typeName);
		void construct(MonoObject* object);

		void onDestroyed();

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