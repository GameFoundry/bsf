#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsStringTable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Manages string tables used for localizing text. Allows you to add 
	 *			and remove different tables and change the active language.
	 */
	class BS_CORE_EXPORT StringTableManager : public Module<StringTableManager>
	{
	public:
		StringTableManager();

		/**
		 * @brief	Gets the currently active language.
		 */
		Language getActiveLanguage() const { return mActiveLanguage; }

		/**
		 * @brief	Changes the currently active language.
		 *			Any newly created strings will use this value.
		 */
		void setActiveLanguage(Language language);

		/**
		 * @brief	Returns the string table with the specified id.
		 *			If the table doesn't exist new one is created.
		 */
		HStringTable getTable(UINT32 id);

		/**
		 * @brief	Removes the string table with the specified id.
		 */
		void removeTable(UINT32 id);

		/**
		 * @brief	Registers a new string table or replaces an old one at the specified id.
		 */
		void setTable(UINT32 id, HStringTable table);

	private:
		Language mActiveLanguage;
		UnorderedMap<UINT32, HStringTable> mTables;
	};
}