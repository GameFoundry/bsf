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
			BS::GUIElementStyle* labelStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

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
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), true);
		}

		static T* create(const BS::GUIContent& labelContent, const BS::GUIOptions& layoutOptions, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), true);
		}

		static T* create(const CM::HString& labelText, CM::UINT32 labelWidth, const BS::GUIOptions& layoutOptions, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(labelText), labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), true);
		}

		static T* create(const CM::HString& labelText, const BS::GUIOptions& layoutOptions, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), true);
		}

		static T* create(const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(), 0, nullptr, entryElementStyle, 
				GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), false);
		}

		static T* create(const BS::GUIContent& labelContent, CM::UINT32 labelWidth, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(&GUISkin::DefaultStyle), true);
		}

		static T* create(const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(&GUISkin::DefaultStyle), true);
		}

		static T* create(const CM::HString& labelText, CM::UINT32 labelWidth, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(labelText), labelWidth, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(&GUISkin::DefaultStyle), true);
		}

		static T* create(const CM::HString& labelText, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, entryElementStyle, 
				GUILayoutOptions::create(&GUISkin::DefaultStyle), true);
		}

		static T* create(BS::GUIElementStyle* entryElementStyle = nullptr)
		{
			return cm_new<T>(PrivatelyConstruct(), BS::GUIContent(), 0, nullptr, entryElementStyle, 
				GUILayoutOptions::create(&GUISkin::DefaultStyle), false);
		}

		TGUIField(const PrivatelyConstruct& dummy, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			BS::GUIElementStyle* labelStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel)
			:GUIFieldBase(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel)
		{ }
	};
}