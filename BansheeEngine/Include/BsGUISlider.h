#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	A slider with a draggable handle that can be vertical or horizontal.
	 */
	class BS_EXPORT GUISlider : public GUIElementContainer
	{
	public:
		/**
		 * @brief	Name of the style for the handle button used by the slider.
		 */
		static const String& getHandleStyleType();

		/**
		 * @brief	Name of the style for the background image used by the slider.
		 */
		static const String& getBackgroundStyleType();

		/**
		 * @brief	Moves the slider handle the the specified position in the handle area.
		 *
		 * @param	pct	Position to move the handle to, in percent ranging [0.0f, 1.0f]
		 */
		void setPercent(float pct);

		/**
		 * @brief	Gets the current position of the slider handle, in percent ranging [0.0f, 1.0f].
		 */
		float getPercent() const;

		/**
		 * @copydoc	GUIElementContainer::_getOptimalSize
		 */
		virtual Vector2I _getOptimalSize() const;

		Event<void(float percent)> onChanged;
	protected:
		GUISlider(bool horizontal, const String& styleName, const GUILayoutOptions& layoutOptions);
		virtual ~GUISlider();

		/**
		 * @copydoc	GUIElementContainer::_getOptimalSize
		 */
		virtual void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		/**
		 * @brief	Triggered when the slider handles moves.
		 */
		void onHandleMoved(float newPosition);

	private:
		GUISliderHandle* mSliderHandle;
		GUITexture* mBackground;

		HEvent mHandleMovedConn;
	};

	/**
	 * @brief	A horizontal slider with a draggable handle.
	 */
	class BS_EXPORT GUISliderHorz : public GUISlider
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new horizontal slider.
		 *
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUISliderHorz* create(const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new horizontal slider.
		 *
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUISliderHorz* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

	private:
		GUISliderHorz(const String& styleName, const GUILayoutOptions& layoutOptions);
	};

	/**
	 * @brief	A vertical slider with a draggable handle.
	 */
	class BS_EXPORT GUISliderVert : public GUISlider
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new vertical slider.
		 *
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUISliderVert* create(const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new vertical slider.
		 *
		 * @param	layoutOptions	Options that allows you to control how is the element positioned in
		 *							GUI layout. This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUISliderVert* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

	private:
		GUISliderVert(const String& styleName, const GUILayoutOptions& layoutOptions);
	};
}