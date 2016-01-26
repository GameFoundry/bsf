//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

namespace BansheeEngine
{
	/**
	 * @brief	Helper class used for constructing HString%s that references the engine string table.
	 *			Engine string table is just a separate string table so it doesn't conflict with
	 *			the game string table.
	 */
	class BS_EXPORT HEString
	{
	public:
		/**
		 * @brief	Creates a new localized string with the specified identifier in the engine string table. 
		 *			If the identifier doesn't previously exist in the string table, identifier value will also 
		 *			be used for initializing the default language version of the string.
		 *
		 * @param	identifier		String you can use for later referencing the localized string.
		 */
		HEString(const WString& identifier);

		/**
		 * @brief	Creates a new localized string with the specified identifier in the engine string table and sets the 
		 *			default language version of the string. If a string with that identifier already exists default language 
		 *			string will be updated.
		 *
		 * @param	identifier		String you can use for later referencing the localized string.
		 * @param	default			Default string to assign to the specified identifier. Language to which it
		 *							will be assigned depends on the StringTable::DEFAULT_LANGUAGE value.
		 */
		HEString(const WString& identifier, const WString& default);

		/**
		 * @brief	Creates a new empty localized string in the engine string table.
		 */
		HEString();

		/**
		 * @brief	Implicitly casts the editor string type to a generic string type.
		 */
		operator HString() const;

	private:
		static const UINT32 ENGINE_STRING_TABLE_ID;

		HString mInternal;
	};
}