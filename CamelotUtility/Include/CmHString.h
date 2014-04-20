#pragma once

#include <boost/signals.hpp>

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
	class CM_UTILITY_EXPORT HString
	{
	public:
		class CM_UTILITY_EXPORT StringData
		{
		public:
			StringData();
			~StringData();

			mutable boost::signal<void()> onStringModified;

		private:
			friend class HString;

			LocalizedStringData* mStringData;
			WString* mParameters;
			boost::signals::connection mUpdateConn;

			mutable bool mIsDirty;
			mutable WString mCachedString;
			mutable WString* mStringPtr;

			void updateString();
		};

		/**
		 * @brief	Creates a new localized string with the specified identifier. If the identifier
		 * 			doesn't previously exist in the string table, identifier value will also be used 
		 * 			for initializing the English version of the string.
		 */
		explicit HString(const WString& identifierString);
		HString();
		HString(const HString& copy);
		~HString();

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
		 * @brief	Registers a callback that gets triggered whenever string value changes. This may happen
		 * 			when the string table is modified, or when the active language is changed.
		 */
		boost::signals::connection addOnStringModifiedCallback(std::function<void()> callback) const;

		/**
		 * @brief	Returns an empty string.
		 */
		static const HString& dummy();
	private:
		std::shared_ptr<StringData> mData;
	};
}