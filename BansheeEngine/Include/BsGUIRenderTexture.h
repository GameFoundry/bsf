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

		static GUIRenderTexture* create(const CM::RenderTexturePtr& texture, const GUIElementStyle* style = nullptr);
		static GUIRenderTexture* create(const CM::RenderTexturePtr& texture, const GUIOptions& layoutOptions, const GUIElementStyle* style = nullptr);

	protected:
		const CM::RenderTexture* mSourceTexture;

		GUIRenderTexture(const GUIElementStyle* style, const CM::RenderTexturePtr& texture, const GUILayoutOptions& layoutOptions);
		virtual ~GUIRenderTexture();

		/**
		 * @copydoc GUIElement::updateRenderElementsInternal()
		 */
		virtual void updateRenderElementsInternal();
	};
}