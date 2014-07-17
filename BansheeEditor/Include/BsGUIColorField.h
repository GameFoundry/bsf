#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsColor.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIColorField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();
		static const String& getLabelStyleType();
		static const String& getColorInputStyleType();

		static GUIColorField* create(const GUIContent& labelContent, 
			const GUIOptions& layoutOptions, const String& style = StringUtil::BLANK);

		static GUIColorField* create(const HString& labelText, 
			const GUIOptions& layoutOptions, const String& style = StringUtil::BLANK);

		static GUIColorField* create(const GUIOptions& layoutOptions, const String& style = StringUtil::BLANK);

		static GUIColorField* create(const GUIContent& labelContent, const String& style = StringUtil::BLANK);

		static GUIColorField* create(const HString& labelText, const String& style = StringUtil::BLANK);

		static GUIColorField* create(const String& style = StringUtil::BLANK);

		GUIColorField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
			const String& style, const GUILayoutOptions& layoutOptions);

		GUIColorField(const PrivatelyConstruct& dummy, const String& style, const GUILayoutOptions& layoutOptions);

		Color getValue() const { return mValue; }
		void setValue(const Color& value);

		void setLabelWidth(UINT32 width);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

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