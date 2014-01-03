#include "BsScriptGUIContent.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptHString.h"
#include "BsScriptGUIContent.h"
#include "BsScriptSpriteTexture.h"
#include "BsSpriteTexture.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	MonoField* ScriptGUIContent::mTextField;
	MonoField* ScriptGUIContent::mTooltipField;
	MonoField* ScriptGUIContent::mImageField;

	void ScriptGUIContent::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "GUIContent", &ScriptGUIContent::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptGUIContent::initRuntimeData()
	{
		mTextField = &metaData.scriptClass->getField("_text");
		mTooltipField = &metaData.scriptClass->getField("_tooltip");
		mImageField = &metaData.scriptClass->getField("_image");
	}

	const CM::HString& ScriptGUIContent::getText(MonoObject* instance)
	{
		MonoObject* textManaged = (MonoObject*)mTextField->getValue(instance);

		if(textManaged == nullptr)
			return HString::dummy();

		ScriptHString* textScript = ScriptHString::toNative(textManaged);
		return textScript->getInternalValue();
	}

	const CM::HString& ScriptGUIContent::getTooltip(MonoObject* instance)
	{
		MonoObject* tooltipManaged = (MonoObject*)mTooltipField->getValue(instance);

		if(tooltipManaged == nullptr)
			return HString::dummy();

		ScriptHString* tooltipScript = ScriptHString::toNative(tooltipManaged);
		return tooltipScript->getInternalValue();
	}

	HSpriteTexture ScriptGUIContent::getImage(MonoObject* instance)
	{
		MonoObject* imageManaged = (MonoObject*)mImageField->getValue(instance);

		if(imageManaged == nullptr)
			return HSpriteTexture();

		ScriptSpriteTexture* imageScript = ScriptSpriteTexture::toNative(imageManaged);
		return imageScript->getInternalValue();
	}
}