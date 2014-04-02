#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIIntField : public BS::GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const CM::String& getGUITypeName();

		static GUIIntField* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIIntField* create(BS::GUIWidget& parent, const CM::HString& labelText, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIIntField* create(BS::GUIWidget& parent, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIIntField* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIIntField* create(BS::GUIWidget& parent, const CM::HString& labelText, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIIntField* create(BS::GUIWidget& parent, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		GUIIntField(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions);

		GUIIntField(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, 
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions);

		CM::INT32 getValue() const;
		void setValue(CM::INT32 value);

	protected:
		virtual ~GUIIntField();

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);
	protected:
		static const float SPLIT_POSITION;

		BS::GUILabel* mLabel;
		BS::GUIInputBox* mInputBox;

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);

		static bool intFilter(const CM::WString& str);
	};
}