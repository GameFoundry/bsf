#pragma once

#include <boost/signals.hpp>

namespace CamelotFramework
{
	/**
	 * @brief	String handle. Provides a wrapper around an Unicode string, primarily
	 * 			for localization purposes.
	 * 			
	 *			Actual value for this string is looked up in a global string table based
	 *			on the provided identifier string and currently active language. If such value 
	 *			doesn't exist then the identifier is used as is.
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

		explicit HString(const WString& identifierString);
		HString();
		HString(const HString& copy);
		~HString();

		operator const WString& () const;
		const WString& getValue() const;

		void setParameter(UINT32 idx, const WString& value);
		
		boost::signals::connection addOnStringModifiedCallback(std::function<void()> callback) const;

		static const HString& dummy();
	private:
		std::shared_ptr<StringData> mData;
	};
}