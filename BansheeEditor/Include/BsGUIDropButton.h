#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIButtonBase.h"
#include "BsGUIToggleGroup.h"
#include "BsImageSprite.h"
#include "BsTextSprite.h"
#include "BsGUIContent.h"
#include "boost/signal.hpp"

namespace BansheeEditor
{
	class GUIDropButton : public BS::GUIButtonBase
	{
	public:
		static const CM::String& getGUITypeName();

		static GUIDropButton* create(CM::UINT32 dragType, const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIDropButton* create(CM::UINT32 dragType, const BS::GUIOptions& layoutOptions, 
			const CM::String& styleName = CM::StringUtil::BLANK);

		boost::signal<void(void*)> onDataDropped;
	protected:
		virtual ~GUIDropButton();

	protected:
		GUIDropButton(CM::UINT32 dragType, const CM::String& styleName, const BS::GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);

		CM::UINT32 mDragType;
	};
}