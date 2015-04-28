#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT GUIGameObjectField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& options, 
			const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const GUIContent& labelContent, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const HString& labelText, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const HString& labelText, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const GUIOptions& options, const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const GUIContent& labelContent,
			const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const HString& labelText,
			const String& style = StringUtil::BLANK);

		static GUIGameObjectField* create(const String& typeNamespace, const String& type, const String& style = StringUtil::BLANK);

		GUIGameObjectField(const PrivatelyConstruct& dummy, const String& typeNamespace, const String& type, const GUIContent& labelContent,
			UINT32 labelWidth, const String& style, const GUIDimensions& dimensions, bool withLabel);

		HGameObject getValue() const;
		void setValue(const HGameObject& value);

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		Vector2I _getOptimalSize() const;

		Event<void(const HGameObject&)> onValueChanged;
	private:
		virtual ~GUIGameObjectField();

		void styleUpdated();

		void dataDropped(void* data);
		void onClearButtonClicked();

	private:
		static const UINT32 DEFAULT_LABEL_WIDTH;

		GUILayout* mLayout;
		GUILabel* mLabel;
		GUIDropButton* mDropButton;
		GUIButton* mClearButton;
		String mType;
		String mNamespace;

		UINT64 mInstanceId;
	};
}