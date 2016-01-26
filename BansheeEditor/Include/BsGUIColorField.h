#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIColorField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();

		static GUIColorField* create(const GUIContent& labelContent, 
			const GUIOptions& layoutOptions, const String& labelStyle = StringUtil::BLANK, const String& toggleStyle = StringUtil::BLANK);

		static GUIColorField* create(const HString& labelText, 
			const GUIOptions& layoutOptions, const String& labelStyle = StringUtil::BLANK, const String& toggleStyle = StringUtil::BLANK);

		static GUIColorField* create(const GUIOptions& layoutOptions, const String& labelStyle = StringUtil::BLANK, 
			const String& toggleStyle = StringUtil::BLANK);

		static GUIColorField* create(const GUIContent& labelContent, const String& labelStyle = StringUtil::BLANK, 
			const String& toggleStyle = StringUtil::BLANK);

		static GUIColorField* create(const HString& labelText, const String& labelStyle = StringUtil::BLANK,
			const String& toggleStyle = StringUtil::BLANK);

		static GUIColorField* create(const String& labelStyle = StringUtil::BLANK, const String& toggleStyle = StringUtil::BLANK);

		GUIColorField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
			const String& labelStyle, const String& toggleStyle, const GUILayoutOptions& layoutOptions);

		GUIColorField(const PrivatelyConstruct& dummy, const String& labelStyle, const String& toggleStyle, 
			const GUILayoutOptions& layoutOptions);

		Color getValue() const;
		void setValue(const Color& value);

		void setLabelWidth(UINT32 width);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		Vector2I _getOptimalSize() const;
	protected:
		virtual ~GUIColorField();

	protected:
		UINT32 mLabelWidth;
		GUILabel* mLabel;
		GUIColor* mColor;
	};
}