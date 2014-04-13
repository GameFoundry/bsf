#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIFieldBase : public BS::GUIElementContainer
	{
	protected:
		struct PrivatelyConstruct {};

	public:
		GUIFieldBase(const PrivatelyConstruct& dummy, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			const CM::String& labelStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		CM::Vector2I _getOptimalSize() const;
	protected:
		virtual ~GUIFieldBase() { }

	protected:
		static const CM::UINT32 DEFAULT_LABEL_WIDTH;

		BS::GUILayout* mLayout;
		BS::GUILabel* mLabel;
	};

	template <class T>
	class TGUIField : public GUIFieldBase
	{
	public:
		static T* create(const BS::GUIContent& labelContent, CM::UINT32 labelWidth, const BS::GUIOptions& layoutOptions, 
			const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), true);
		}

		static T* create(const BS::GUIContent& labelContent, const BS::GUIOptions& layoutOptions, 
			const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), true);
		}

		static T* create(const CM::HString& labelText, CM::UINT32 labelWidth, const BS::GUIOptions& layoutOptions, 
			const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(labelText), labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), true);
		}

		static T* create(const CM::HString& labelText, const BS::GUIOptions& layoutOptions, 
			const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), true);
		}

		static T* create(const BS::GUIOptions& layoutOptions, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(), 0, nullptr, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions), false);
		}

		static T* create(const BS::GUIContent& labelContent, CM::UINT32 labelWidth, 
			const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(), true);
		}

		static T* create(const BS::GUIContent& labelContent, 
			const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(), true);
		}

		static T* create(const CM::HString& labelText, CM::UINT32 labelWidth, 
			const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(labelText), labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(), true);
		}

		static T* create(const CM::HString& labelText, 
			const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(), true);
		}

		static T* create(const CM::String& entryElementStyle = CM::StringUtil::BLANK)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(), 0, nullptr, entryElementStyle, 
				GUILayoutOptions::create(), false);
		}

		TGUIField(const PrivatelyConstruct& dummy, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			const CM::String& labelStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel)
			:GUIFieldBase(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel)
		{ }
	};
}