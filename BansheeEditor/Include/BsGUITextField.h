#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUITextField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getLabelStyleType();
		static const String& getGUITypeName();
		static const String& getInputStyleType();

		static GUITextField* create(bool multiline, const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& layoutOptions,
			const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const GUIContent& labelContent, const GUIOptions& layoutOptions,
			const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const HString& labelText, UINT32 labelWidth, const GUIOptions& layoutOptions,
			const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const HString& labelText, const GUIOptions& layoutOptions,
			const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const GUIOptions& layoutOptions, const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const GUIContent& labelContent,
			const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const HString& labelText,
			const String& style = StringUtil::BLANK);

		static GUITextField* create(bool multiline, const String& style = StringUtil::BLANK);

		GUITextField(const PrivatelyConstruct& dummy, bool multiline, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUILayoutOptions& layoutOptions, bool withLabel);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		Vector2I _getOptimalSize() const;

		WString getValue() const { return mValue; }
		void setValue(const WString& value);

		bool hasInputFocus() const { return mHasInputFocus; }

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		Event<void(const WString&)> onValueChanged;
	protected:
		static const UINT32 DEFAULT_LABEL_WIDTH;

		virtual ~GUITextField();

		void styleUpdated();

		void valueChanged(const WString& newValue);
		void focusGained();
		void focusLost();

		GUIInputBox* mInputBox;
		GUILayout* mLayout;
		GUILabel* mLabel;
		bool mHasInputFocus;
		WString mValue;
	};
}