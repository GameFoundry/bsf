#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIFieldBase : public GUIElementContainer
	{
	protected:
		struct PrivatelyConstruct {};

	public:
		GUIFieldBase(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const GUIDimensions& layoutOptions, bool withLabel);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		virtual Vector2I _getOptimalSize() const;

		static const String& getLabelStyleType()
		{
			static String LABEL_STYLE_TYPE = "EditorFieldLabel";
			return LABEL_STYLE_TYPE;
		}

	protected:
		virtual ~GUIFieldBase() { }

		virtual void styleUpdated();

		static const UINT32 DEFAULT_LABEL_WIDTH;

		GUILayout* mLayout;
		GUILabel* mLabel;
	};

	template <class T>
	class TGUIField : public GUIFieldBase
	{
	public:
		static T* create(const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& layoutOptions,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), labelContent, labelWidth, *curStyle,
				GUIDimensions::create(layoutOptions), true);
		}

		static T* create(const GUIContent& labelContent, const GUIOptions& layoutOptions,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
				GUIDimensions::create(layoutOptions), true);
		}

		static T* create(const HString& labelText, UINT32 labelWidth, const GUIOptions& layoutOptions,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, *curStyle,
				GUIDimensions::create(layoutOptions), true);
		}

		static T* create(const HString& labelText, const GUIOptions& layoutOptions,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
				GUIDimensions::create(layoutOptions), true);
		}

		static T* create(const GUIOptions& layoutOptions, const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(), 0, *curStyle,
				GUIDimensions::create(layoutOptions), false);
		}

		static T* create(const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), labelContent, labelWidth, *curStyle, GUIDimensions::create(), true);
		}

		static T* create(const GUIContent& labelContent,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
				GUIDimensions::create(), true);
		}

		static T* create(const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, *curStyle,
				GUIDimensions::create(), true);
		}

		static T* create(const HString& labelText,
			const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
				GUIDimensions::create(), true);
		}

		static T* create(const String& style = StringUtil::BLANK)
		{
			const String* curStyle = &style;
			if (*curStyle == StringUtil::BLANK)
				curStyle = &T::getGUITypeName();

			return bs_new<T>(PrivatelyConstruct(), GUIContent(), 0, *curStyle,
				GUIDimensions::create(), false);
		}

		TGUIField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& layoutOptions, bool withLabel)
			:GUIFieldBase(dummy, labelContent, labelWidth, style, layoutOptions, withLabel)
		{ }
	};
}