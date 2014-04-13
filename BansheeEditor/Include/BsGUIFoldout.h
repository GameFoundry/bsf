#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIFoldout : public BS::GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const CM::String& getGUITypeName();

		static GUIFoldout* create(const BS::GUIOptions& layoutOptions, 
			const CM::String& toggleStyle = CM::StringUtil::BLANK, const CM::String& backgroundStyle = CM::StringUtil::BLANK);

		static GUIFoldout* create(const CM::String& toggleStyle = CM::StringUtil::BLANK,
			const CM::String& backgroundStyle = CM::StringUtil::BLANK);

		GUIFoldout(const PrivatelyConstruct& dummy, const CM::String& toggleStyle, 
			const CM::String& backgroundStyle, const BS::GUILayoutOptions& layoutOptions);

		bool isExpanded() const { return mIsExpanded; }
		void setExpanded(bool expanded);

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		CM::Vector2I _getOptimalSize() const;

		boost::signal<void(bool)> onStateChanged;
	protected:
		virtual ~GUIFoldout();

	protected:
		static const CM::String FOLDOUT_BUTTON_STYLE;
		static const CM::String FOLDOUT_BG_STYLE;

		BS::GUIToggle* mToggle;
		BS::GUITexture* mBackground;

		bool mIsExpanded;
	};
}