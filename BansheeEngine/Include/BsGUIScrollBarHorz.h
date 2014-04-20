#pragma once

#include "BsPrerequisites.h"
#include "BsGUIScrollBar.h"

#include "boost/signal.hpp"

namespace BansheeEngine
{
	class BS_EXPORT GUIScrollBarHorz : public GUIScrollBar
	{
	public:
		static const String& getGUITypeName();

		static GUIScrollBarHorz* create(const String& styleName = StringUtil::BLANK);
		static GUIScrollBarHorz* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);
	protected:
		GUIScrollBarHorz(const String& styleName, const GUILayoutOptions& layoutOptions);
		~GUIScrollBarHorz();
	};
}