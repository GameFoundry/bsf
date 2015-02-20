#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"
#include "BsVector2.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIVector2Field : public TGUIField<GUIVector2Field>
	{
	public:
		static const String& getGUITypeName();
		static const String& getFloatFieldStyleType();

		GUIVector2Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUILayoutOptions& layoutOptions, bool withLabel);

		Vector2 getValue() const;
		void setValue(const Vector2& value);

		bool hasInputFocus() const;

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		Event<void(const Vector2&)> onValueChanged;
	protected:
		virtual ~GUIVector2Field() { }

	protected:
		static const UINT32 ELEMENT_LABEL_WIDTH;

		void styleUpdated();
		void valueChanged(float newValue);

		GUIFloatField* mFieldX;
		GUIFloatField* mFieldY;
	};
}