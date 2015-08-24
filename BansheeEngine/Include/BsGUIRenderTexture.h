#pragma once

#include "BsPrerequisites.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	Allows you to display a render texture in the GUI. Has the same functionality as GUITexture, but also forwards
	 *			any input to underlying GUI elements being rendered on the provided render texture.
	 */
	class BS_EXPORT GUIRenderTexture : public GUITexture
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new element with the provided render texture.
		 *
		 * @param	texture			Render texture to display.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIRenderTexture* create(const RenderTexturePtr& texture, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new element with the provided render texture.
		 *
		 * @param	texture			Render texture to display.
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIRenderTexture* create(const RenderTexturePtr& texture, const GUIOptions& options, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Changes the active render texture whose contents to display in the GUI element.
		 */
		void setRenderTexture(const RenderTexturePtr& texture);

	protected:
		GUIRenderTexture(const String& styleName, const RenderTexturePtr& texture, const GUIDimensions& dimensions);
		virtual ~GUIRenderTexture();

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal
		 */
		virtual void updateRenderElementsInternal() override;

		RenderTexturePtr mSourceTexture;
	};
}