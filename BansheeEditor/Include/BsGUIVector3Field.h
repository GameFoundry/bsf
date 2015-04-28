#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIFieldBase.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIVector3Field : public TGUIField<GUIVector3Field>
	{
	public:
		static const String& getGUITypeName();
		static const String& getFloatFieldStyleType();

		GUIVector3Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style, const GUIDimensions& layoutOptions, bool withLabel);

		Vector3 getValue() const;
		void setValue(const Vector3& value);

		bool hasInputFocus() const;

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		Event<void(const Vector3&)> onValueChanged;
	protected:
		virtual ~GUIVector3Field() { }

		void styleUpdated();
		void valueChanged(float newValue);

		static const UINT32 ELEMENT_LABEL_WIDTH;

		GUIFloatField* mFieldX;
		GUIFloatField* mFieldY;
		GUIFloatField* mFieldZ;
	};
}