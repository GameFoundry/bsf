#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIGameObjectField : public BS::GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const CM::String& getGUITypeName();

		static GUIGameObjectField* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, CM::UINT32 labelWidth, const BS::GUIOptions& layoutOptions, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, const BS::GUIOptions& layoutOptions, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, const CM::HString& labelText, CM::UINT32 labelWidth, const BS::GUIOptions& layoutOptions, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, const CM::HString& labelText, const BS::GUIOptions& layoutOptions, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, CM::UINT32 labelWidth, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, const CM::HString& labelText, CM::UINT32 labelWidth, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, const CM::HString& labelText, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		static GUIGameObjectField* create(BS::GUIWidget& parent, BS::GUIElementStyle* dropButtonStyle = nullptr,
			BS::GUIElementStyle* clearButtonStyle = nullptr);

		GUIGameObjectField(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			CM::UINT32 labelWidth, BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle,
			BS::GUIElementStyle* clearButtonStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

		CM::HGameObject getValue() const;
		void setValue(const CM::HGameObject& value);

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		CM::Vector2I _getOptimalSize() const;
	private:
		virtual ~GUIGameObjectField();

		void dataDropped(void* data);

	private:
		static const CM::UINT32 DEFAULT_LABEL_WIDTH;

		BS::GUILayout* mLayout;
		BS::GUILabel* mLabel;
		GUIDropButton* mDropButton;
		BS::GUIButton* mClearButton;

		CM::UINT64 mInstanceId;
	};
}