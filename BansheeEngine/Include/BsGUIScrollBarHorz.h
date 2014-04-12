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

		static GUIScrollBarHorz* create(const GUIElementStyle* style = nullptr);
		static GUIScrollBarHorz* create(const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);
	protected:
		GUIScrollBarHorz(const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);
		~GUIScrollBarHorz();
	};
}