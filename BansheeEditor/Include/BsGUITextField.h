#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUITextField : public TGUIField<GUITextField>
	{
	public:
		static const String& getGUITypeName();

		GUITextField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel);

		WString getValue() const;
		void setValue(const WString& value);
	protected:
		virtual ~GUITextField();

	protected:
		GUIInputBox* mInputBox;
	};
}