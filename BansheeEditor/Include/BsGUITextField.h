#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUITextField : public TGUIField<GUITextField>
	{
	public:
		static const String& getGUITypeName();
		static const String& getInputStyleType();

		GUITextField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUILayoutOptions& layoutOptions, bool withLabel);

		WString getValue() const { return mValue; }
		void setValue(const WString& value);

		bool hasInputFocus() const { return mHasInputFocus; }

		Event<void(const WString&)> onValueChanged;
	protected:
		virtual ~GUITextField();

		void styleUpdated();

		void valueChanged(const WString& newValue);
		void focusGained();
		void focusLost();

		GUIInputBox* mInputBox;
		bool mHasInputFocus;
		WString mValue;
	};
}