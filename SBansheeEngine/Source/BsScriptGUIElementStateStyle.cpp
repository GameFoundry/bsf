#include "BsScriptGUIElementStateStyle.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptResourceManager.h"
#include "BsGUIElementStyle.h"

namespace BansheeEngine
{
	MonoField* ScriptGUIElementStateStyle::sTextureField = nullptr;
	MonoField* ScriptGUIElementStateStyle::sTextColorField = nullptr;

	ScriptGUIElementStateStyle::ScriptGUIElementStateStyle(MonoObject* instance)
		:ScriptObject(instance)
	{

	}

	void ScriptGUIElementStateStyle::initRuntimeData()
	{
		sTextureField = metaData.scriptClass->getField("Texture");
		sTextColorField = metaData.scriptClass->getField("TextColor");
	}

	MonoObject* ScriptGUIElementStateStyle::toManaged(const GUIElementStyle::GUIElementStateStyle& state)
	{
		ScriptSpriteTexture* scriptTexture = nullptr;

		if (state.texture != nullptr)
			ScriptResourceManager::instance().getScriptResource(state.texture, &scriptTexture, true);

		MonoObject* instance = metaData.scriptClass->createInstance();

		if (scriptTexture != nullptr)
			sTextureField->setValue(instance, scriptTexture->getManagedInstance());
		else
			sTextureField->setValue(instance, nullptr);

		sTextColorField->setValue(instance, (void*)&state.textColor);
		return instance;
	}

	GUIElementStyle::GUIElementStateStyle ScriptGUIElementStateStyle::toNative(MonoObject* instance)
	{
		GUIElementStyle::GUIElementStateStyle output;

		if (instance == nullptr)
			return output;

		MonoObject* monoTexture = nullptr;
		sTextureField->getValue(instance, &monoTexture);

		if (monoTexture != nullptr)
		{
			ScriptSpriteTexture* scriptTexture = ScriptSpriteTexture::toNative(monoTexture);
			output.texture = scriptTexture->getHandle();
		}

		sTextColorField->getValue(instance, &output.textColor);

		return output;
	}
}