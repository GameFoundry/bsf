#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIToggleField : public TGUIField<GUIToggleField>
	{
	public:
		static const String& getGUITypeName();
		static const String& getToggleStyleType();

		GUIToggleField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUILayoutOptions& layoutOptions, bool withLabel);

		bool getValue() const { return mValue; }
		void setValue(bool value);

		Event<void(bool)> onValueChanged;
	protected:
		virtual ~GUIToggleField() { }

		void styleUpdated();
		void valueChanged(bool newValue);

		GUIToggle* mToggle;
		bool mValue;
	};
}