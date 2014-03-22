#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>
#include <mono/metadata/class.h>

namespace BansheeEngine
{
	enum class MonoFieldVisibility
	{
		Private,
		ProtectedInternal,
		Internal,
		Protected,
		Public
	};

	class BS_MONO_EXPORT MonoField
	{
	public:
		const CM::String& getName() const { return mName; }
		MonoClass* getType();

		void getValue(MonoObject* instance, void* outValue);
		void setValue(MonoObject* instance, void* value);

		bool hasAttribute(MonoClass* monoClass);
		MonoObject* getAttribute(MonoClass* monoClass);

		MonoFieldVisibility getVisibility();
		bool isStatic();

	private:
		friend class MonoClass;

		MonoField(MonoClassField* field);

		CM::String mName;
		MonoClassField* mField;
		MonoClass* mFieldType;
		void* mThunk;
	};
}