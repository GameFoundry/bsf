#pragma once

#include "BsPrerequisites.h"
#include "BsGUIScrollBar.h"

#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBarVert : public GUIScrollBar
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIScrollBarVert* create(const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIScrollBarVert* create(const GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);
	protected:
		GUIScrollBarVert(const CM::String& styleName, const GUILayoutOptions& layoutOptions);
		~GUIScrollBarVert();
	};
}