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

		static GUIScrollBarVert* create(GUIWidget& parent, const GUIElementStyle* style = nullptr);
		static GUIScrollBarVert* create(GUIWidget& parent, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);
	protected:
		GUIScrollBarVert(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);
		~GUIScrollBarVert();
	};
}