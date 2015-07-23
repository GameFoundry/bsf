#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITexture.h"
#include "BsImageSprite.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI element used for displaying a border on an editor window edge.
	 */
	class GUIWindowFrame : public GUITexture
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new GUI window frame element.
		 *
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIWindowFrame* create(const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new GUI window frame element.
		 *
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIWindowFrame* create(const GUIOptions& options, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Sets whether the frame should be displayed in focus or unfocused state.
		 */
		void setFocused(bool focused);
	protected:
		~GUIWindowFrame();
		GUIWindowFrame(const String& styleName, const GUIDimensions& dimensions);
	};
}