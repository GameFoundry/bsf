//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI object that displays a field in which a Texture can be dragged and dropped.
	 *			The field accepts a Texture of a specific type and displays an optional label.
	 *			If texture is referenced its image is displayed in the field.
	 */
	class BS_SCR_BED_EXPORT GUITextureField : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new texture GUI editor field with a label.
		 *
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field with a label.
		 *
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const GUIContent& labelContent, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field with a label.
		 *
		 * @param	labelText		Text to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const HString& labelText, UINT32 labelWidth, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field with a label.
		 *
		 * @param	labelText		Text to display in the editor field label.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const HString& labelText, const GUIOptions& options,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field without a label.
		 *
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const GUIOptions& options, const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field with a label.
		 *
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const GUIContent& labelContent, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field with a label.
		 *
		 * @param	labelContent	Content to display in the editor field label.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const GUIContent& labelContent,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field with a label.
		 *
		 * @param	labelText		Text to display in the editor field label.
		 * @param	labelWidth		Width of the label in pixels.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const HString& labelText, UINT32 labelWidth,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field with a label.
		 *
		 * @param	labelText		Text to display in the editor field label.
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const HString& labelText,
			const String& style = StringUtil::BLANK);

		/**
		 * @brief	Creates a new texture GUI editor field without a label.
		 *
		 * @param	style			Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUITextureField* create(const String& style = StringUtil::BLANK);

		GUITextureField(const PrivatelyConstruct& dummy, const GUIContent& labelContent,
			UINT32 labelWidth, const String& style, const GUIDimensions& dimensions, bool withLabel);

		/**
		 * @brief	Returns the texture referenced by the field, if any. This will load the texture if it is not already
		 * 			loaded.
		 */
		HTexture getValue() const;

		/**
		 * @brief	Sets the texture referenced by the field.
		 */
		void setValue(const HTexture& value);

		/**
		 * @brief	Returns a weak reference to the texture referenced by the field, if any.
		 */
		WeakResourceHandle<Texture> getValueWeak() const;

		/**
		 * @brief	Sets a weak reference to the texture referenced by the field.
		 */
		void setValueWeak(const WeakResourceHandle<Texture>& value);

		/**
		 * @brief	Returns the texture referenced by the field. Returns
		 *			empty string with no texture is referenced.
		 */
		String getUUID() const { return mUUID; }

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color) override;

		/**
		 * @copydoc	GUIElement::_updateLayoutInternal
		 */
		void _updateLayoutInternal(const GUILayoutData& data) override;

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		Vector2I _getOptimalSize() const override;

		/**
		 * @brief	Triggered whenever the referenced texture changes. Provides
		 *			a weak handle to the resource, or empty handle if no texture is referenced.
		 */
		Event<void(const WeakResourceHandle<Texture>&)> onValueChanged;
	private:
		virtual ~GUITextureField();

		/**
		 * @copydoc	GUIElement::styleUpdated
		 */
		void styleUpdated() override;

		/**
		 * @brief	Sets the texture referenced by the field by finding
		 *			the texture with the provided UUID.
		 *			
		 * @param	uuid			Unique resource identifier of the texture to show, or empty string if no texture.
		 * @param	triggerEvent	Determines should the ::onValueChanged event be triggered if the new UUID is different 
		 * 							from the previous one.
		 */
		void setUUID(const String& uuid, bool triggerEvent = true);

		/**
		 * @brief	Triggered when a drag and drop operation finishes over this element.
		 */
		void dataDropped(void* data);

		/**
		 * @brief	Triggered when the drop button that displays the game object label is clicked.
		 */
		void onDropButtonClicked();

		/**
		 * @brief	Triggered when the clear button is clicked.
		 */
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