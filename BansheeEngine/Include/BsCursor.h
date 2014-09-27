#pragma once

#include "BsPrerequisites.h"
#include "BsModule.h"
#include "BsPixelData.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Allows you to manipulate the platform cursor in various ways.
	 * 			
	 * @note	Thread safe.
	 */
	class BS_EXPORT Cursor : public Module<Cursor>
	{
		/**
		 * @brief	Internal container for data about a single cursor icon.
		 */
		struct CustomIcon
		{
			CustomIcon() {}
			CustomIcon(const PixelData& pixelData, const Vector2I& hotSpot)
				:hotSpot(hotSpot), pixelData(pixelData)
			{ }

			Vector2I hotSpot;
			PixelData pixelData;
		};

	public:
		Cursor();

		/**
		 * @brief	Moves the cursor to the specified screen position.
		 */
		void setScreenPosition(const Vector2I& screenPos);

		/**
		 * @brief	Retrieves the cursor position in screen coordinates.
		 */
		Vector2I getScreenPosition();

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
		void clipToWindow(const RenderWindow& window);

		/**
		 * @brief	Limit cursor movement to specific area on the screen.
		 */
		void clipToRect(const Rect2I& screenRect);

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
		void setCursor(const String& name);

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
		void setCursorIcon(const String& name, const PixelData& pixelData, const Vector2I& hotSpot);

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
		void setCursorIcon(CursorType type, const PixelData& pixelData, const Vector2I& hotSpot);

		/**
		 * @brief	Removes a custom cursor icon and releases any data associated with it.
		 */
		void clearCursorIcon(const String& name);

		/**
		 * @brief	Removes a custom cursor icon and releases any data associated with it. Restores
		 * 			original icon associated with this cursor type.
		 */
		void clearCursorIcon(CursorType type);

	private:
		/**
		 * @brief	Restores the default cursor icon for the specified cursor type.
		 */
		void restoreCursorIcon(CursorType type);

		/**
		 * @brief	Sends the cursor image to the OS, making it active.
		 */
		void updateCursorImage();

		UnorderedMap<String, UINT32> mCustomIconNameToId;
		UnorderedMap<UINT32, CustomIcon> mCustomIcons;
		UINT32 mNextUniqueId;
		INT32 mActiveCursorId;
	};
}