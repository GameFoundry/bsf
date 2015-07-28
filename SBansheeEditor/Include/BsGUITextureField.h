#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT GUITextureField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();

		static GUITextureField* create(const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUITextureField* create(const GUIContent& labelContent, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUITextureField* create(const HString& labelText, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUITextureField* create(const HString& labelText, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		static GUITextureField* create(const GUIOptions& options, const String& style = StringUtil::BLANK);

		static GUITextureField* create(const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		static GUITextureField* create(const GUIContent& labelContent,
			const String& style = StringUtil::BLANK);

		static GUITextureField* create(const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		static GUITextureField* create(const HString& labelText,
			const String& style = StringUtil::BLANK);

		static GUITextureField* create(const String& style = StringUtil::BLANK);

		GUITextureField(const PrivatelyConstruct& dummy, const GUIContent& labelContent,
			UINT32 labelWidth, const String& style, const GUIDimensions& dimensions, bool withLabel);

		HTexture getValue() const;
		void setValue(const HTexture& value);

		String getUUID() const { return mUUID; }
		void setUUID(const String& uuid);

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color) override;

		void _updateLayoutInternal(const GUILayoutData& data) override;

		Vector2I _getOptimalSize() const override;

		Event<void(const String&)> onValueChanged;
	private:
		virtual ~GUITextureField();

		void styleUpdated() override;

		void dataDropped(void* data);
		void onClearButtonClicked();

	private:
		static const UINT32 DEFAULT_LABEL_WIDTH;

		GUILayout* mLayout;
		GUILabel* mLabel;
		GUIDropButton* mDropButton;
		GUIButton* mClearButton;
		String mUUID;
	};
}