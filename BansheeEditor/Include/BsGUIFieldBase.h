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
			const String& labelStyle, const GUILayoutOptions& layoutOptions, bool withLabel);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		Vector2I _getOptimalSize() const;
	protected:
		virtual ~GUIFieldBase() { }

	protected:
		static const UINT32 DEFAULT_LABEL_WIDTH;

		GUILayout* mLayout;
		GUILabel* mLabel;
	};

	template <class T>
	class TGUIField : public GUIFieldBase
	{
	public:
		static T* create(const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& layoutOptions, 
			const String& labelStyle = StringUtil::BLANK, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), true);
		}

		static T* create(const GUIContent& labelContent, const GUIOptions& layoutOptions, 
			const String& labelStyle = StringUtil::BLANK, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), true);
		}

		static T* create(const HString& labelText, UINT32 labelWidth, const GUIOptions& layoutOptions, 
			const String& labelStyle = StringUtil::BLANK, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), true);
		}

		static T* create(const HString& labelText, const GUIOptions& layoutOptions, 
			const String& labelStyle = StringUtil::BLANK, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), true);
		}

		static T* create(const GUIOptions& layoutOptions, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), GUIContent(), 0, nullptr, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), false);
		}

		static T* create(const GUIContent& labelContent, UINT32 labelWidth, 
			const String& labelStyle = StringUtil::BLANK, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(), true);
		}

		static T* create(const GUIContent& labelContent, 
			const String& labelStyle = StringUtil::BLANK, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(), true);
		}

		static T* create(const HString& labelText, UINT32 labelWidth, 
			const String& labelStyle = StringUtil::BLANK, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(), true);
		}

		static T* create(const HString& labelText, 
			const String& labelStyle = StringUtil::BLANK, const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(), true);
		}

		static T* create(const String& entryElementStyle = StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), GUIContent(), 0, nullptr, entryElementStyle, 
				GUILayoutOptions::create(), false);
		}

		TGUIField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
			:GUIFieldBase(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel)
		{ }
	};
}