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
		static const CM::String& getGUITypeName();

		static GUIRenderTexture* create(const CM::RenderTexturePtr& texture, const CM::String& styleName = CM::StringUtil::BLANK);
		static GUIRenderTexture* create(const CM::RenderTexturePtr& texture, const GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

	protected:
		const CM::RenderTexture* mSourceTexture;

		GUIRenderTexture(const CM::String& styleName, const CM::RenderTexturePtr& texture, const GUILayoutOptions& layoutOptions);
		virtual ~GUIRenderTexture();

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();
	};
}