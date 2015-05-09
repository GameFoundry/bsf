#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIComponentFoldout : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();
		static const String& getFoldoutButtonStyleType();

		static GUIComponentFoldout* create(const HString& label, const GUIOptions& options, const String& style = StringUtil::BLANK);
		static GUIComponentFoldout* create(const HString& label, const String& style = StringUtil::BLANK);

		GUIComponentFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style, const GUIDimensions& dimensions);

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
		virtual ~GUIComponentFoldout();

		void toggleTriggered(bool value);
		void styleUpdated();

		GUIToggle* mToggle;

		bool mIsExpanded;
	};
}