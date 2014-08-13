#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsComponent.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ManagedComponent : public Component
	{
	public:
		MonoObject* getManagedInstance() const { return mManagedInstance; }
		MonoReflectionType* getRuntimeType() const { return mRuntimeType; }

		const String& getManagedNamespace() const { return mNamespace; }
		const String& getManagedTypeName() const { return mTypeName; }
		const String& getManagedFullTypeName() const { return mFullTypeName; }

	private:
		MonoObject* mManagedInstance;
		MonoReflectionType* mRuntimeType;
		uint32_t mManagedHandle;

		String mNamespace;
		String mTypeName;
		String mFullTypeName;

		/************************************************************************/
		/* 							COMPONENT OVERRIDES                    		*/
		/************************************************************************/

	protected:
		friend class SceneObject;

		/** Standard constructor.
        */
		ManagedComponent(const HSceneObject& parent, MonoReflectionType* runtimeType);
		void construct(MonoObject* object, MonoReflectionType* runtimeType);

		void onDestroyed();

	public:
		virtual void update() {}

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class ManagedComponentRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	protected:
		ManagedComponent() {} // Serialization only
	};
}