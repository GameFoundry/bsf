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
		static const String& getFoldoutButtonStyleType();
		static const String& getLabelStyleType();

		static GUIFoldout* create(const HString& label, const GUIOptions& layoutOptions, const String& style = StringUtil::BLANK);
		static GUIFoldout* create(const HString& label, const String& style = StringUtil::BLANK);

		GUIFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style, const GUILayoutOptions& layoutOptions);

		bool isExpanded() const { return mIsExpanded; }
		void setExpanded(bool expanded);

		/**
		 * Changes the label of the foldout.
		 */
		void setContent(const GUIContent& content);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		Vector2I _getOptimalSize() const;

		Event<void(bool)> onStateChanged;
	protected:
		virtual ~GUIFoldout();

		void toggleTriggered(bool value);
		void styleUpdated();

		GUILabel* mLabel;
		GUIToggle* mToggle;

		bool mIsExpanded;
	};
}