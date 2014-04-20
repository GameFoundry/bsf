#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT GUIGameObjectField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();

		static GUIGameObjectField* create(const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& layoutOptions, 
			const String& labelStyle = StringUtil::BLANK, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const GUIContent& labelContent, const GUIOptions& layoutOptions, 
			const String& labelStyle = StringUtil::BLANK, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const HString& labelText, UINT32 labelWidth, const GUIOptions& layoutOptions, 
			const String& labelStyle = StringUtil::BLANK, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const HString& labelText, const GUIOptions& layoutOptions, 
			const String& labelStyle = StringUtil::BLANK, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const GUIOptions& layoutOptions, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const GUIContent& labelContent, UINT32 labelWidth, 
			const String& labelStyle = StringUtil::BLANK, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const GUIContent& labelContent, 
			const String& labelStyle = StringUtil::BLANK, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const HString& labelText, UINT32 labelWidth, 
			const String& labelStyle = StringUtil::BLANK, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const HString& labelText, 
			const String& labelStyle = StringUtil::BLANK, const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& dropButtonStyle = StringUtil::BLANK,
			const String& clearButtonStyle = StringUtil::BLANK);

		GUIGameObjectField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
			UINT32 labelWidth, const String& labelStyle, const String& dropButtonStyle,
			const String& clearButtonStyle, const GUILayoutOptions& layoutOptions, bool withLabel);

		HGameObject getValue() const;
		void setValue(const HGameObject& value);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		Vector2I _getOptimalSize() const;
	private:
		virtual ~GUIGameObjectField();

		void dataDropped(void* data);

	private:
		static const UINT32 DEFAULT_LABEL_WIDTH;
		static const String DROP_BUTTON_STYLE;
		static const String CLEAR_BUTTON_STYLE;

		GUILayout* mLayout;
		GUILabel* mLabel;
		GUIDropButton* mDropButton;
		GUIButton* mClearButton;

		UINT64 mInstanceId;
	};
}