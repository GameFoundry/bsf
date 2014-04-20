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
		static const String& getGUITypeName();

		static GUIRenderTexture* create(const RenderTexturePtr& texture, const String& styleName = StringUtil::BLANK);
		static GUIRenderTexture* create(const RenderTexturePtr& texture, const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

	protected:
		const RenderTexture* mSourceTexture;

		GUIRenderTexture(const String& styleName, const RenderTexturePtr& texture, const GUILayoutOptions& layoutOptions);
		virtual ~GUIRenderTexture();

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();
	};
}