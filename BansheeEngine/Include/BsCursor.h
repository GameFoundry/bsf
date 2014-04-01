#pragma once

#include "BsPrerequisites.h"
#include "CmModule.h"
#include "CmPixelData.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	enum class CursorType
	{
		Arrow,
		ArrowDrag,
		ArrowLeftRight,
		Wait,
		IBeam,
		SizeNESW,
		SizeNS,
		SizeNWSE,
		SizeWE,
		Deny,

		// Keep at the end
		Count
	};

	/**
	 * @brief	Allows you to manipulate the platform cursor in various ways.
	 * 			
	 * @note	Thread safe.
	 */
	class BS_EXPORT Cursor : public CM::Module<Cursor>
	{
		struct CustomIcon
		{
			CustomIcon() {}
			CustomIcon(const CM::PixelData& pixelData, const CM::Vector2I& hotSpot)
				:hotSpot(hotSpot), pixelData(pixelData)
			{ }

			CM::Vector2I hotSpot;
			CM::PixelData pixelData;
		};

	public:
		Cursor();

		/**
		 * @brief	Moves the cursor to the specified screen position.
		 */
		void move(const CM::Vector2I& screenPos);

		/**
		 * @brief	Hides the cursor.
		 */
		void hide();

		/**
		 * @brief	Shows the cursor.
		 */
		void show();

		/**
		 * @brief	Limit cursor movement to the specified window.
		 */
		void clipToWindow(const CM::RenderWindow& window);

		/**
		 * @brief	Limit cursor movement to specific area on the screen.
		 */
		void clipToRect(const CM::RectI& screenRect);

		/**
		 * @brief	Disables cursor clipping that was set using any of the "clipTo*" methods.
		 */
		void clipDisable();
		
		/**
		 * @brief	Sets a cursor icon. Uses one of the built-in cursor types.
		 */
		void setCursor(CursorType type);

		/**
		 * @brief	Sets a cursor icon. Uses one of the manually registered icons.
		 * 			
		 * @param	name		The name to identify the cursor, one set previously by calling "addCursorIcon".
		 */
		void setCursor(const CM::String& name);

		/**
		 * @brief	Registers a new custom cursor icon you can then set by calling "setCursor".
		 *
		 * @param	name		The name to identify the cursor.
		 * @param 	pixelData	Cursor image data.
		 * @param	hotSpot		Offset on the cursor image to where the actual input happens (e.g. tip of the Arrow cursor).
		 * 						
		 * @note	Stores an internal copy of the pixel data. Clear it by calling "removeCursorIcon".
		 * 			If a custom icon with the same name already exists it will be replaced.
		 */
		void setCursorIcon(const CM::String& name, const CM::PixelData& pixelData, const CM::Vector2I& hotSpot);

		/**
		 * @brief	Registers a new custom cursor icon you can then set by calling "setCursor".
		 *
		 * @param	type		One of the built-in cursor types.
		 * @param 	pixelData	Cursor image data.
		 * @param	hotSpot		Offset on the cursor image to where the actual input happens (e.g. tip of the Arrow cursor).
		 * 						
		 * @note	Stores an internal copy of the pixel data. Clear it by calling "removeCursorIcon".
		 * 			If a custom icon with the same type already exists it will be replaced.
		 */
		void setCursorIcon(CursorType type, const CM::PixelData& pixelData, const CM::Vector2I& hotSpot);

		/**
		 * @brief	Removes a custom cursor icon and releases any data associated with it.
		 */
		void clearCursorIcon(const CM::String& name);

		/**
		 * @brief	Removes a custom cursor icon and releases any data associated with it. Restores
		 * 			original icon associated with this cursor type.
		 */
		void clearCursorIcon(CursorType type);

	private:
		CM::UnorderedMap<CM::String, CM::UINT32>::type mCustomIconNameToId;
		CM::UnorderedMap<CM::UINT32, CustomIcon>::type mCustomIcons;
		CM::UINT32 mNextUniqueId;
		CM::UINT32 mActiveCursorId;

		void restoreCursorIcon(CursorType type);
	};
}