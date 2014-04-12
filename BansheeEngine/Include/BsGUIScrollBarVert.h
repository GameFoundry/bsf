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

		static GUIScrollBarVert* create(const GUIElementStyle* style = nullptr);
		static GUIScrollBarVert* create(const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);
	protected:
		GUIScrollBarVert(const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);
		~GUIScrollBarVert();
	};
}