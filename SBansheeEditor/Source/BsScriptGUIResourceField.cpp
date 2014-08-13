#include "BsScriptGUIResourceField.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsSpriteTexture.h"
#include "BsMonoUtil.h"
#include "BsGUILayout.h"
#include "BsGUIResourceField.h"
#include "BsGUIOptions.h"
#include "BsGUIContent.h"
#include "BsScriptGUIElementStyle.h"
#include "BsScriptGUILayout.h"
#include "BsScriptGUIArea.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"
#include "BsScriptResource.h"
#include "BsScriptResourceManager.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptGUIResourceField::OnChangedThunkDef ScriptGUIResourceField::onChangedThunk;

	ScriptGUIResourceField::ScriptGUIResourceField(MonoObject* instance, GUIResourceField* resourceField)
		:TScriptGUIElement(instance, resourceField)
	{

	}

	void ScriptGUIResourceField::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptGUIResourceField::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetValue", &ScriptGUIResourceField::internal_getValue);
		metaData.scriptClass->addInternalCall("Internal_SetValue", &ScriptGUIResourceField::internal_setValue);

		onChangedThunk = (OnChangedThunkDef)metaData.scriptClass->getMethod("DoOnChanged", 1).getThunk();
	}

	void ScriptGUIResourceField::internal_createInstance(MonoObject* instance, MonoReflectionType* type, MonoObject* title, UINT32 titleWidth,
		MonoString* style, MonoArray* guiOptions, bool withTitle)
	{
		GUIOptions options;

		UINT32 arrayLen = (UINT32)mono_array_length(guiOptions);
		for (UINT32 i = 0; i < arrayLen; i++)
			options.addOption(mono_array_get(guiOptions, GUIOption, i));

		String styleName = toString(MonoUtil::monoToWString(style));

		MonoType* monoType = mono_reflection_type_get_type(type);
		::MonoClass* monoClass = mono_type_get_class(monoType);

		String typeNamespace = mono_class_get_namespace(monoClass);
		String typeName = mono_class_get_name(monoClass);

		GUIResourceField* guiResourceField = nullptr;
		if (withTitle)
		{
			GUIContent nativeContent(ScriptGUIContent::getText(title), ScriptGUIContent::getImage(title), ScriptGUIContent::getTooltip(title));
			guiResourceField = GUIResourceField::create(typeNamespace, typeName, nativeContent, titleWidth, options, styleName);
		}
		else
		{
			guiResourceField = GUIResourceField::create(typeNamespace, typeName, options, styleName);
		}

		guiResourceField->onValueChanged.connect(std::bind(&ScriptGUIResourceField::onChanged, instance, _1));

		ScriptGUIResourceField* nativeInstance = new (bs_alloc<ScriptGUIResourceField>()) ScriptGUIResourceField(instance, guiResourceField);
	}

	void ScriptGUIResourceField::internal_getValue(ScriptGUIResourceField* nativeInstance, MonoObject** output)
	{
		GUIResourceField* resourceField = static_cast<GUIResourceField*>(nativeInstance->getGUIElement());

		HResource resource = resourceField->getValue();
		*output = nativeToManagedResource(resource);
	}

	void ScriptGUIResourceField::internal_setValue(ScriptGUIResourceField* nativeInstance, MonoObject* value)
	{
		GUIResourceField* resourceField = static_cast<GUIResourceField*>(nativeInstance->getGUIElement());

		if (value == nullptr)
			resourceField->setValue(HResource());
		else
		{
			ScriptResourceBase* scriptResource = ScriptResource::toNative(value);
			resourceField->setValue(scriptResource->getNativeHandle());
		}
	}

	void ScriptGUIResourceField::onChanged(MonoObject* instance, const HResource& newValue)
	{
		MonoException* exception = nullptr;
		MonoObject* managedObj = nativeToManagedResource(newValue);

		onChangedThunk(instance, managedObj, &exception);

		MonoUtil::throwIfException(exception);
	}

	MonoObject* ScriptGUIResourceField::nativeToManagedResource(const HResource& instance)
	{
		if (instance == nullptr)
		{
			return nullptr;
		}
		else
		{
			ScriptResourceBase* scriptResource = ScriptResourceManager::instance().getScriptResource(instance.getUUID());
			if (scriptResource == nullptr)
				return nullptr;
			else
				return scriptResource->getManagedInstance();
		}
	}
}