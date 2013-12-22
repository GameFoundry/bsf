#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIWidget.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptGUIWidget : public ScriptObject<ScriptGUIWidget>
	{
	public:
		static void initMetaData();

		static ScriptGUIWidget* create(const CM::HSceneObject& sceneObject);
	private:
		static void internal_createInstance(MonoObject* instance, CM::SceneObject* sceneObject);
		static void internal_destroyInstance(ScriptGUIWidget* nativeInstance);

		static void initRuntimeData();

		ScriptGUIWidget(const HGUIWidget& widget);

		HGUIWidget mWidget;
	};
}