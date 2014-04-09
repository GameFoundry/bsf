#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUITextField : public TGUIField<GUITextField>
	{
	public:
		static const CM::String& getGUITypeName();

		GUITextField(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* inputBoxStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

		CM::WString getValue() const;
		void setValue(const CM::WString& value);
	protected:
		virtual ~GUITextField();

	protected:
		BS::GUIInputBox* mInputBox;
	};
}