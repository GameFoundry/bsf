//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	Allows you to display a render texture in the GUI.
	 * 			
	 * @note	Has the same functionality as GUITexture, but also forwards any input to the control
	 * 			to underlying GUI elements.
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
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIRenderTexture* create(const RenderTexturePtr& texture, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

	protected:
		GUIRenderTexture(const String& styleName, const RenderTexturePtr& texture, const GUILayoutOptions& layoutOptions);
		virtual ~GUIRenderTexture();

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal
		 */
		virtual void updateRenderElementsInternal();

		const RenderTexture* mSourceTexture;
	};
}