#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIToggleField : public BS::GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const CM::String& getGUITypeName();

		static GUIToggleField* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIToggleField* create(BS::GUIWidget& parent, const CM::HString& labelText, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIToggleField* create(BS::GUIWidget& parent, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIToggleField* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIToggleField* create(BS::GUIWidget& parent, const CM::HString& labelText, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIToggleField* create(BS::GUIWidget& parent, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		GUIToggleField(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* toggleStyle, const BS::GUILayoutOptions& layoutOptions);

		GUIToggleField(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, 
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* toggleStyle, const BS::GUILayoutOptions& layoutOptions);

		bool getValue() const;
		void setValue(bool value);

		void setLabelWidth(CM::UINT32 width);

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		CM::Vector2I _getOptimalSize() const;
	protected:
		virtual ~GUIToggleField();

	protected:
		CM::UINT32 mLabelWidth;
		BS::GUILabel* mLabel;
		BS::GUIToggle* mToggle;
	};
}