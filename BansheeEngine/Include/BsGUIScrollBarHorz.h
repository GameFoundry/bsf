//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsGUIScrollBar.h"

namespace BansheeEngine
{
	/**
	 * @brief	Specialization of a horizontal scroll bar.
	 */
	class BS_EXPORT GUIScrollBarHorz : public GUIScrollBar
	{
	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new horizontal scroll bar.
		 *
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIScrollBarHorz* create(const String& styleName = StringUtil::BLANK);

		/**
		 * @brief	Creates a new horizontal scroll bar.
		 *
		 * @param	options			Options that allow you to control how is the element positioned and sized.
		 *							This will override any similar options set by style.
		 * @param	styleName		Optional style to use for the element. Style will be retrieved
		 *							from GUISkin of the GUIWidget the element is used on. If not specified
		 *							default style is used.
		 */
		static GUIScrollBarHorz* create(const GUIOptions& options, const String& styleName = StringUtil::BLANK);
	protected:
		GUIScrollBarHorz(const String& styleName, const GUIDimensions& dimensions);
		~GUIScrollBarHorz();
	};
}