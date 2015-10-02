#include "BsScriptSerializableUtility.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsScriptAssemblyManager.h"
#include "BsManagedSerializableField.h"
#include "BsMemorySerializer.h"

namespace BansheeEngine
{
	ScriptSerializableUtility::ScriptSerializableUtility(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptSerializableUtility::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Clone", &ScriptSerializableUtility::internal_Clone);
	}

	MonoObject* ScriptSerializableUtility::internal_Clone(MonoObject* original)
	{
		if (original == nullptr)
			return nullptr;

		::MonoClass* monoClass = mono_object_get_class(original);
		MonoClass* engineClass = MonoManager::instance().findClass(monoClass);

		ManagedSerializableTypeInfoPtr typeInfo = ScriptAssemblyManager::instance().getTypeInfo(engineClass);
		ManagedSerializableFieldDataPtr data = ManagedSerializableFieldData::create(typeInfo, original);

		MemorySerializer ms;

		// Note: This code unnecessarily encodes to binary and decodes from it. I could have added a specialized clone method that does it directly,
		// but didn't feel the extra code was justified.
		UINT32 size = 0;
		UINT8* encodedData = ms.encode(data.get(), size);
		ManagedSerializableFieldDataPtr clonedData = std::static_pointer_cast<ManagedSerializableFieldData>(ms.decode(encodedData, size));

		return clonedData->getValueBoxed(typeInfo);
	}
}