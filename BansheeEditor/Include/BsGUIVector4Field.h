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
		static const String& getFloatFieldStyleType();

		GUIVector4Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& dimensions, bool withLabel);

		Vector4 getValue() const;
		void setValue(const Vector4& value);

		bool hasInputFocus() const;

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		Event<void(const Vector4&)> onValueChanged;
	protected:
		virtual ~GUIVector4Field() { }

		void styleUpdated();
		void valueChanged(float newValue);

		static const UINT32 ELEMENT_LABEL_WIDTH;

		GUIFloatField* mFieldX;
		GUIFloatField* mFieldY;
		GUIFloatField* mFieldZ;
		GUIFloatField* mFieldW;
	};
}