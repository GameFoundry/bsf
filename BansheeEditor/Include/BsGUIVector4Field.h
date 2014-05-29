#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"
#include "BsVector4.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIVector4Field : public TGUIField<GUIVector4Field>
	{
	public:
		static const String& getGUITypeName();

		GUIVector4Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel);

		Vector4 getValue() const;
		void setValue(const Vector4& value);

	protected:
		virtual ~GUIVector4Field() { }

	protected:
		static const UINT32 ELEMENT_LABEL_WIDTH;

		GUIFloatField* mFieldX;
		GUIFloatField* mFieldY;
		GUIFloatField* mFieldZ;
		GUIFloatField* mFieldW;
	};
}