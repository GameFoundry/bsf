#pragma once

#include "BsPrerequisites.h"
#include "BsResource.h"
#include "BsGUIElementStyle.h"

namespace BansheeEngine
{
	/**
	 * @brief	Holds a set of styles that control how are GUI element types positioned and
	 *			displayed in the GUI. Each element type can be assigned a specific style.
	 */
	class BS_EXPORT GUISkin : public Resource
	{
	public:
		/**
		 * @brief	Returns a style for the specified GUI element type.
		 *
		 * @see		GUIElement::getGUITypeName
		 */
		const GUIElementStyle* getStyle(const String& guiElemType) const;

		/**
		 * @brief	Sets a style for the specified GUI element type.
		 *
		 * @see		GUIElement::getGUITypeName
		 */
		void setStyle(const String& guiElemType, const GUIElementStyle& style);

		/**
		 * @brief	Removes a style for the specified GUI element type.
		 *
		 * @see		GUIElement::getGUITypeName
		 */
		void removeStyle(const String& guiElemType);

		/**
		 * @brief	Returns names of all styles registered on this skin.
		 */
		Vector<String> getStyleNames() const;

		/**
		 * @brief	Default style that may be used when no other is available.
		 */
		static GUIElementStyle DefaultStyle;

		/**
		 * @brief	Creates an empty GUI skin and returns a handle to it.
		 */
		static HGUISkin create();

		/**
		 * @brief	Creates an empty GUI skin and returns a pointer to it.
		 *
		 * @note	Internal method. Use "create" returning handle for normal use.
		 */
		static GUISkinPtr _createPtr();

	private:
		GUISkin();
		GUISkin(const GUISkin& skin); // Disable copying

		UnorderedMap<String, GUIElementStyle> mStyles;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GUISkinRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}