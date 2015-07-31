#pragma once

namespace BansheeEngine
{
	/**
	 * @brief	String handle. Provides a wrapper around an Unicode string, primarily
	 * 			for localization purposes.
	 * 			
	 *			Actual value for this string is looked up in a global string table based
	 *			on the provided identifier string and currently active language. If such value 
	 *			doesn't exist then the identifier is used as is.
	 *			
	 *			Use {0}, {1}, etc. in the string value for values that might change dynamically.
	 */
	class BS_CORE_EXPORT HString
	{
	public:
		/**
		 * @brief	Creates a new localized string with the specified identifier. If the identifier
		 * 			doesn't previously exist in the string table, identifier value will also be used 
		 * 			for initializing the default language version of the string.
		 *
		 * @param	identifier		String you can use for later referencing the localized string.
		 * @param	stringTableId	Unique identifier of the string table to retrieve the string from.
		 */
		explicit HString(const WString& identifier, UINT32 stringTableId = 0);

		/**
		 * @brief	Creates a new localized string with the specified identifier and sets the default language version
		 *			of the string. If a string with that identifier already exists default language string will be updated.
		 *
		 * @param	identifier		String you can use for later referencing the localized string.
		 * @param	default			Default string to assign to the specified identifier. Language to which it
		 *							will be assigned depends on the StringTable::DEFAULT_LANGUAGE value.
		 * @param	stringTableId	Unique identifier of the string table to retrieve the string from.
		 */
		explicit HString(const WString& identifier, const WString& default, UINT32 stringTableId = 0);

		/**
		 * @brief	Creates a new empty localized string.
		 *
		 * @param	stringTableId	Unique identifier of the string table to retrieve the string from.
		 */
		HString(UINT32 stringTableId = 0);
		HString(const HString& copy);
		~HString();

		HString& operator=(const HString& rhs);

		operator const WString& () const;
		const WString& getValue() const;

		/**
		 * @brief	Sets a value of a string parameter. Parameters are specified as bracketed values
		 * 			within the string itself (e.g. {0}, {1}) etc.
		 *
		 * @note	Useful for strings that have dynamically changing values, like numbers, embedded in them.
		 */
		void setParameter(UINT32 idx, const WString& value);
		
		/**
		 * @brief	Returns an empty string.
		 */
		static const HString& dummy();
	private:
		SPtr<LocalizedStringData> mStringData;
		WString* mParameters;

		mutable bool mIsDirty;
		mutable WString mCachedString;
		mutable WString* mStringPtr;
	};
}