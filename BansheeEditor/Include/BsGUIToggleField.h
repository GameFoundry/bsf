#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIToggleField : public TGUIField<GUIToggleField>
	{
	public:
		static const CM::String& getGUITypeName();

		GUIToggleField(const PrivatelyConstruct& dummy, BS::GUIWidget& parent, const BS::GUIContent& labelContent, CM::UINT32 labelWidth,
			BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* toggleStyle, const BS::GUILayoutOptions& layoutOptions, bool withLabel);

		bool getValue() const;
		void setValue(bool value);

	protected:
		virtual ~GUIToggleField() { }

	protected:
		BS::GUIToggle* mToggle;
	};
}