#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains a background image and a fill image that
	 *			is scaled depending on the percentage set by the user.
	 */
	class BS_EXPORT GUIProgressBar : public GUIElementContainer
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Name of the style for the fill image used by the progress bar.
		 */
		static const String& getBarStyleType();

		/**
		 * @brief	Name of the style for the background image used by the progress bar.
		 */
		static const String& getBackgroundStyleType();

		/**
		 * @brief	Creates a new progress bar.
		 *
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIProgressBar* create(const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new progress bar.
		 *
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIProgressBar* create(const GUIOptions& options, const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Fills up the progress bar up to the specified percentage.
		 *
		 * @param	pct	How far to extend the fill image, in percent ranging [0.0f, 1.0f]
		 */
		void setPercent(float pct);

		/**
		 * @brief	Gets the percentage of how full is the progress bar currently.
		 */
		float getPercent() const { return mPercent; }

		/**
		 * @copydoc	GUIElement::setTint
		 */
		virtual void setTint(const Color& color);

		/**
		 * @copydoc	GUIElementContainer::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const;

		Event<void(float percent)> onChanged;
	protected:
		GUIProgressBar(const String& styleName, const GUIDimensions& dimensions);

		/**
		 * @copydoc	GUIElementContainer::_updateLayoutInternal
		 */
		virtual void _updateLayoutInternal(const GUILayoutData& data);

		/**
		 * @copydoc	GUIElementContainer::styleUpdated
		 */
		void styleUpdated();

	private:
		GUITexture* mBar;
		GUITexture* mBackground;

		float mPercent;
	};
}