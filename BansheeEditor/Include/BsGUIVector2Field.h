#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"
#include "CmVector2.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIVector2Field : public TGUIField<GUIVector2Field>
	{
	public:
		static const CM::String& getGUITypeName();

		GUIVector2Field(const PrivatelyConstruct& dummy, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

		CM::Vector2 getValue() const;
		void setValue(const CM::Vector2& value);

	protected:
		virtual ~GUIVector2Field() { }

	protected:
		static const CM::UINT32 ELEMENT_LABEL_WIDTH;

		GUIFloatField* mFieldX;
		GUIFloatField* mFieldY;
	};
}