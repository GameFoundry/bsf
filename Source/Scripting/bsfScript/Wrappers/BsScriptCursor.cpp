//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Wrappers/BsScriptCursor.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"
#include "Platform/BsCursor.h"
#include "Generated/BsScriptPixelData.generated.h"

namespace bs
{
	ScriptCursor::ScriptCursor(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptCursor::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetScreenPosition", (void*)&ScriptCursor::internal_getScreenPosition);
		metaData.scriptClass->addInternalCall("Internal_SetScreenPosition", (void*)&ScriptCursor::internal_setScreenPosition);
		metaData.scriptClass->addInternalCall("Internal_Hide", (void*)&ScriptCursor::internal_hide);
		metaData.scriptClass->addInternalCall("Internal_Show", (void*)&ScriptCursor::internal_show);
		metaData.scriptClass->addInternalCall("Internal_ClipToRect", (void*)&ScriptCursor::internal_clipToRect);
		metaData.scriptClass->addInternalCall("Internal_ClipDisable", (void*)&ScriptCursor::internal_clipDisable);
		metaData.scriptClass->addInternalCall("Internal_SetCursor", (void*)&ScriptCursor::internal_setCursor);
		metaData.scriptClass->addInternalCall("Internal_SetCursorStr", (void*)&ScriptCursor::internal_setCursorStr);
		metaData.scriptClass->addInternalCall("Internal_SetCursorIcon", (void*)&ScriptCursor::internal_setCursorIcon);
		metaData.scriptClass->addInternalCall("Internal_SetCursorIconStr", (void*)&ScriptCursor::internal_setCursorIconStr);
		metaData.scriptClass->addInternalCall("Internal_ClearCursorIcon", (void*)&ScriptCursor::internal_clearCursorIcon);
		metaData.scriptClass->addInternalCall("Internal_ClearCursorIconStr", (void*)&ScriptCursor::internal_clearCursorIconStr);
	}

	void ScriptCursor::internal_getScreenPosition(Vector2I* value)
	{
		Cursor::instance().getScreenPosition();
	}

	void ScriptCursor::internal_setScreenPosition(Vector2I* value)
	{
		Cursor::instance().setScreenPosition(*value);
	}

	void ScriptCursor::internal_hide()
	{
		Cursor::instance().hide();
	}

	void ScriptCursor::internal_show()
	{
		Cursor::instance().show();
	}

	void ScriptCursor::internal_clipToRect(Rect2I* value)
	{
		Cursor::instance().clipToRect(*value);
	}

	void ScriptCursor::internal_clipDisable()
	{
		Cursor::instance().clipDisable();
	}

	void ScriptCursor::internal_setCursorStr(MonoString* name)
	{
		String nameStr = MonoUtil::monoToString(name);
		Cursor::instance().setCursor(nameStr);
	}

	void ScriptCursor::internal_setCursor(CursorType cursor)
	{
		Cursor::instance().setCursor(cursor);
	}

	void ScriptCursor::internal_setCursorIconStr(MonoString* name, MonoObject* iconData, Vector2I* hotspot)
	{
		String nameStr = MonoUtil::monoToString(name);

		ScriptPixelData* scriptPixelData = ScriptPixelData::toNative(iconData);

		if (scriptPixelData != nullptr)
		{
			SPtr<PixelData> pixelData = scriptPixelData->getInternal();
			Cursor::instance().setCursorIcon(nameStr, *pixelData, *hotspot);
		}
		else
			Cursor::instance().clearCursorIcon(nameStr);
	}

	void ScriptCursor::internal_setCursorIcon(CursorType cursor, MonoObject* iconData, Vector2I* hotspot)
	{
		ScriptPixelData* scriptPixelData = ScriptPixelData::toNative(iconData);

		if (scriptPixelData != nullptr)
		{
			SPtr<PixelData> pixelData = scriptPixelData->getInternal();
			Cursor::instance().setCursorIcon(cursor, *pixelData, *hotspot);
		}
		else
			Cursor::instance().clearCursorIcon(cursor);
	}

	void ScriptCursor::internal_clearCursorIconStr(MonoString* name)
	{
		String nameStr = MonoUtil::monoToString(name);
		Cursor::instance().clearCursorIcon(nameStr);
	}

	void ScriptCursor::internal_clearCursorIcon(CursorType cursor)
	{
		Cursor::instance().clearCursorIcon(cursor);
	}
}
