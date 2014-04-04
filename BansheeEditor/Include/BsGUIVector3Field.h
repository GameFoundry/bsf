#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "CmVector3.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIVector3Field : public BS::GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const CM::String& getGUITypeName();

		static GUIVector3Field* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIVector3Field* create(BS::GUIWidget& parent, const CM::HString& labelText, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIVector3Field* create(BS::GUIWidget& parent, 
			const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIVector3Field* create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIVector3Field* create(BS::GUIWidget& parent, const CM::HString& labelText, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		static GUIVector3Field* create(BS::GUIWidget& parent, 
			BS::GUIElementStyle* labelStyle = nullptr, BS::GUIElementStyle* inputBoxStyle = nullptr);

		GUIVector3Field(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions);

		GUIVector3Field(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, 
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions);

		CM::Vector3 getValue() const;
		void setValue(const CM::Vector3& value);

		CM::Vector2I _getOptimalSize() const;
	protected:
		virtual ~GUIVector3Field();

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

	protected:
		static const CM::UINT32 ELEMENT_LABEL_WIDTH;

		BS::GUILabel* mLabel;
		GUIFloatField* mFieldX;
		GUIFloatField* mFieldY;
		GUIFloatField* mFieldZ;

		void construct(BS::GUIWidget& parent, const BS::GUIContent& labelContent, BS::GUIElementStyle* labelStyle, 
			BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);
	};
}