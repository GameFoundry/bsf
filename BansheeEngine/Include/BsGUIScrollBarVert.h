#pragma once

#include "BsPrerequisites.h"
#include "BsGUIScrollBar.h"

#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBarVert : public GUIScrollBar
	{
	public:
		static const String& getGUITypeName();

		static GUIScrollBarVert* create(const String& styleName = StringUtil::BLANK);
		static GUIScrollBarVert* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);
	protected:
		GUIScrollBarVert(const String& styleName, const GUILayoutOptions& layoutOptions);
		~GUIScrollBarVert();
	};
}