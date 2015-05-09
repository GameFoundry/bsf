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

		static GUIFoldout* create(const HString& label, const GUIOptions& options, const String& style = StringUtil::BLANK);
		static GUIFoldout* create(const HString& label, const String& style = StringUtil::BLANK);

		GUIFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style, const GUIDimensions& dimensions);

		bool isExpanded() const { return mIsExpanded; }
		void setExpanded(bool expanded);

		/**
		 * Changes the label of the foldout.
		 */
		void setContent(const GUIContent& content);

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		void _updateLayoutInternal(const GUILayoutData& data);

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