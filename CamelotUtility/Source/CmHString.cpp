#include "CmHString.h"
#include "CmStringTable.h"

namespace CamelotFramework
{
	HString::HString(const WString& identifierString)
		:mParameters(nullptr)
	{
		mStringData = &StringTable::instance().getStringData(identifierString);
		mParameters = cm_newN<WString>(mStringData->numParameters);
		mUpdateConn = mStringData->commonData->onStringDataModified.connect(boost::bind(&HString::updateString, this, _1));

		updateString(identifierString);
	}

	HString::~HString()
	{
		mUpdateConn.disconnect();

		if(mParameters != nullptr)
			cm_deleteN(mParameters, mStringData->numParameters);
	}

	void HString::setParameter(UINT32 idx, const WString& value)
	{
		mParameters[idx] = value;
	}

	void HString::updateString(const WString& identifierString)
	{
		LocalizedStringData* stringData = &StringTable::instance().getStringData(identifierString);

		// If common data changed re-apply the connections
		if(stringData->commonData != mStringData->commonData)
		{
			mUpdateConn.disconnect();
			mUpdateConn = stringData->commonData->onStringDataModified.connect(boost::bind(&HString::updateString, this, _1));
		}

		mStringData = stringData;
		mCachedString = mStringData->concatenateString(mParameters, mStringData->numParameters);
	}
}