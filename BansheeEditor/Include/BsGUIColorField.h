#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIColorField : public BS::GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const CM::String& getGUITypeName();

		static GUIColorField* create(const BS::GUIContent& labelContent, 
			const BS::GUIOptions& layoutOptions, const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& toggleStyle = CM::StringUtil::BLANK);

		static GUIColorField* create(const CM::HString& labelText, 
			const BS::GUIOptions& layoutOptions, const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& toggleStyle = CM::StringUtil::BLANK);

		static GUIColorField* create(const BS::GUIOptions& layoutOptions, const CM::String& labelStyle = CM::StringUtil::BLANK, 
			const CM::String& toggleStyle = CM::StringUtil::BLANK);

		static GUIColorField* create(const BS::GUIContent& labelContent, const CM::String& labelStyle = CM::StringUtil::BLANK, 
			const CM::String& toggleStyle = CM::StringUtil::BLANK);

		static GUIColorField* create(const CM::HString& labelText, const CM::String& labelStyle = CM::StringUtil::BLANK,
			const CM::String& toggleStyle = CM::StringUtil::BLANK);

		static GUIColorField* create(const CM::String& labelStyle = CM::StringUtil::BLANK, const CM::String& toggleStyle = CM::StringUtil::BLANK);

		GUIColorField(const PrivatelyConstruct& dummy, const BS::GUIContent& labelContent, 
			const CM::String& labelStyle, const CM::String& toggleStyle, const BS::GUILayoutOptions& layoutOptions);

		GUIColorField(const PrivatelyConstruct& dummy, const CM::String& labelStyle, const CM::String& toggleStyle, 
			const BS::GUILayoutOptions& layoutOptions);

		CM::Color getValue() const;
		void setValue(const CM::Color& value);

		void setLabelWidth(CM::UINT32 width);

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		CM::Vector2I _getOptimalSize() const;
	protected:
		virtual ~GUIColorField();

	protected:
		CM::UINT32 mLabelWidth;
		BS::GUILabel* mLabel;
		GUIColor* mColor;
	};
}