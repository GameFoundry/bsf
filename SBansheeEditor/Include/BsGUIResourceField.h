#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT GUIResourceField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();

		static GUIResourceField* create(const String& typeNamespace, const String& type, const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const GUIContent& labelContent, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const HString& labelText, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const HString& labelText, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const GUIOptions& options, const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const GUIContent& labelContent,
			const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const HString& labelText,
			const String& style = StringUtil::BLANK);

		static GUIResourceField* create(const String& typeNamespace, const String& type, const String& style = StringUtil::BLANK);

		GUIResourceField(const PrivatelyConstruct& dummy, const String& typeNamespace, const String& type, const GUIContent& labelContent,
			UINT32 labelWidth, const String& style, const GUIDimensions& dimensions, bool withLabel);

		HResource getValue() const;
		void setValue(const HResource& value);

		String getUUID() const { return mUUID; }
		void setUUID(const String& uuid);

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		void _updateLayoutInternal(const GUILayoutData& data);

		Vector2I _getOptimalSize() const;

		Event<void(const String&)> onValueChanged;
	private:
		virtual ~GUIResourceField();

		void styleUpdated();

		void dataDropped(void* data);
		void onClearButtonClicked();

	private:
		static const UINT32 DEFAULT_LABEL_WIDTH;

		GUILayout* mLayout;
		GUILabel* mLabel;
		GUIDropButton* mDropButton;
		GUIButton* mClearButton;
		String mNamespace;
		String mType;
		String mUUID;
	};
}