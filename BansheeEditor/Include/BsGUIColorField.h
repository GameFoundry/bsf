#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"
#include "BsColor.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIColorField : public TGUIField<GUIColorField>
	{
	public:
		static const String& getGUITypeName();
		static const String& getColorInputStyleType();

		GUIColorField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUILayoutOptions& layoutOptions, bool withLabel);

		Color getValue() const { return mValue; }
		void setValue(const Color& value);

		Vector2I _getOptimalSize() const;

		Event<void(const Color&)> onValueChanged;
	protected:
		virtual ~GUIColorField();

		void styleUpdated();
		void valueChanged(const Color& newValue);

		UINT32 mLabelWidth;
		Color mValue;
		GUILabel* mLabel;
		GUIColor* mColor;
	};
}