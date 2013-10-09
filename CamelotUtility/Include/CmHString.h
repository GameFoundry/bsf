#pragma once

#include "CmPrerequisitesUtil.h"
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
		HString(const WString& identifierString);
		~HString();

		operator const WString& () const { return mCachedString; }
		void setParameter(UINT32 idx, const WString& value);

		boost::signal<void()> onStringModified;
	private:
		LocalizedStringData* mStringData;
		WString* mParameters;
		boost::signals::connection mUpdateConn;

		WString mCachedString;

		void updateString(const WString& identifierString);
	};
}