#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIFoldout : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();

		static GUIFoldout* create(const GUIOptions& layoutOptions, 
			const String& toggleStyle = StringUtil::BLANK, const String& backgroundStyle = StringUtil::BLANK);

		static GUIFoldout* create(const String& toggleStyle = StringUtil::BLANK,
			const String& backgroundStyle = StringUtil::BLANK);

		GUIFoldout(const PrivatelyConstruct& dummy, const String& toggleStyle, 
			const String& backgroundStyle, const GUILayoutOptions& layoutOptions);

		bool isExpanded() const { return mIsExpanded; }
		void setExpanded(bool expanded);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		Vector2I _getOptimalSize() const;

		boost::signal<void(bool)> onStateChanged;
	protected:
		virtual ~GUIFoldout();

	protected:
		static const String FOLDOUT_BUTTON_STYLE;
		static const String FOLDOUT_BG_STYLE;

		GUIToggle* mToggle;
		GUITexture* mBackground;

		bool mIsExpanded;
	};
}