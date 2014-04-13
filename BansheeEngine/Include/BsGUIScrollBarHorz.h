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

		static GUIScrollBarHorz* create(const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIScrollBarHorz* create(const GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);
	protected:
		GUIScrollBarHorz(const CM::String& styleName, const GUILayoutOptions& layoutOptions);
		~GUIScrollBarHorz();
	};
}