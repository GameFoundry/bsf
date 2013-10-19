#pragma once

#include "BsPrerequisites.h"
#include "BsGUIScrollBar.h"

#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBarHorz : public GUIScrollBar
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIScrollBarHorz* create(GUIWidget& parent, const GUIElementStyle* style = nullptr);
		static GUIScrollBarHorz* create(GUIWidget& parent, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);
	protected:
		GUIScrollBarHorz(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);
		~GUIScrollBarHorz();
	};
}