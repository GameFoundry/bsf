#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIColorField : public BS::GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const CM::String& getGUITypeName();

		static GUIColorField* create(const BS::GUIContent& labelContent, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIColorField* create(const CM::HString& labelText, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIColorField* create(const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, 
			BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIColorField* create(const BS::GUIContent& labelContent, BS::GUIElementStyle* labelStyle = nullptr, 
			BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIColorField* create(const CM::HString& labelText, BS::GUIElementStyle* labelStyle = nullptr,
			BS::GUIElementStyle* toggleStyle = nullptr);

		static GUIColorField* create(BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* toggleStyle = nullptr);

		GUIColorField(const PrivatelyConstruct& dummy, const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* toggleStyle, const BS::GUILayoutOptions& layoutOptions);

		GUIColorField(const PrivatelyConstruct& dummy, BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* toggleStyle, 
			const BS::GUILayoutOptions& layoutOptions);

		CM::Color getValue() const;
		void setValue(const CM::Color& value);

		void setLabelWidth(CM::UINT32 width);

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		CM::Vector2I _getOptimalSize() const;
	protected:
		virtual ~GUIColorField();

	protected:
		CM::UINT32 mLabelWidth;
		BS::GUILabel* mLabel;
		GUIColor* mColor;
	};
}