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

		static GUIDropButton* create(BS::GUIWidget& parent, CM::UINT32 dragType, const BS::GUIElementStyle* style = nullptr);
		static GUIDropButton* create(BS::GUIWidget& parent, CM::UINT32 dragType, const BS::GUIOptions& layoutOptions, const BS::GUIElementStyle* style = nullptr);

		boost::signal<void(void*)> onDataDropped;
	protected:
		virtual ~GUIDropButton();

	protected:
		GUIDropButton(BS::GUIWidget& parent, CM::UINT32 dragType, const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);

		CM::UINT32 mDragType;
	};
}