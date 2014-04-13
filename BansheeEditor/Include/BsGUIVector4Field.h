#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"
#include "CmVector4.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIVector4Field : public TGUIField<GUIVector4Field>
	{
	public:
		static const CM::String& getGUITypeName();

		GUIVector4Field(const PrivatelyConstruct& dummy, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			const CM::String& labelStyle, const CM::String& inputBoxStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

		CM::Vector4 getValue() const;
		void setValue(const CM::Vector4& value);

	protected:
		virtual ~GUIVector4Field() { }

	protected:
		static const CM::UINT32 ELEMENT_LABEL_WIDTH;

		GUIFloatField* mFieldX;
		GUIFloatField* mFieldY;
		GUIFloatField* mFieldZ;
		GUIFloatField* mFieldW;
	};
}