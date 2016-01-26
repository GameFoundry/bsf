#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIToggleField : public TGUIField<GUIToggleField>
	{
	public:
		static const String& getGUITypeName();

		GUIToggleField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& labelStyle, const String& toggleStyle, const GUILayoutOptions& layoutOptions, bool withLabel);

		bool getValue() const;
		void setValue(bool value);

	protected:
		virtual ~GUIToggleField() { }

	protected:
		GUIToggle* mToggle;
	};
}