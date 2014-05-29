#include "BsGUISkin.h"
#include "BsGUIElementStyle.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	GUIElementStyle GUISkin::DefaultStyle;

	const GUIElementStyle* GUISkin::getStyle(const String& guiElemType) const
	{
		auto iterFind = mStyles.find(guiElemType);

		if(iterFind != mStyles.end())
			return &iterFind->second;

		LOGWRN("Cannot find GUI style with name: " + guiElemType + ". Returning default style.");

		return &DefaultStyle;
	}

	void GUISkin::setStyle(const String& guiElemType, const GUIElementStyle& style)
	{
		mStyles[guiElemType] = style;
	}
}