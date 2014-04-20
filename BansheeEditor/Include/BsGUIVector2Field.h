#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"
#include "CmVector2.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIVector2Field : public TGUIField<GUIVector2Field>
	{
	public:
		static const String& getGUITypeName();

		GUIVector2Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel);

		Vector2 getValue() const;
		void setValue(const Vector2& value);

	protected:
		virtual ~GUIVector2Field() { }

	protected:
		static const UINT32 ELEMENT_LABEL_WIDTH;

		GUIFloatField* mFieldX;
		GUIFloatField* mFieldY;
	};
}