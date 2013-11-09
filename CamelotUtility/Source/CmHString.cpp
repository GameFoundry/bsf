#include "CmPrerequisitesUtil.h"
#include "CmHString.h"
#include "CmStringTable.h"

namespace CamelotFramework
{
	HString::StringData::StringData()
		:mParameters(nullptr), mIsDirty(true), mStringPtr(nullptr)
	{ }

	HString::StringData::~StringData()
	{
		mUpdateConn.disconnect();

		if(mParameters != nullptr)
			cm_deleteN(mParameters, mStringData->numParameters);
	}

	void HString::StringData::updateString()
	{
		LocalizedStringData* stringData = &StringTable::instance().getStringData(mStringData->commonData->identifier);

		// If common data changed re-apply the connections
		if(stringData->commonData != mStringData->commonData)
		{
			mUpdateConn.disconnect();
			mUpdateConn = stringData->commonData->onStringDataModified.connect(boost::bind(&HString::StringData::updateString, this));
		}

		mStringData = stringData;
		mIsDirty = true;

		onStringModified();
	}

	HString::HString()
	{
		mData = cm_shared_ptr<StringData>();

		mData->mStringData = &StringTable::instance().getStringData(L"");

		if(mData->mStringData->numParameters > 0)
			mData->mParameters = cm_newN<WString>(mData->mStringData->numParameters);

		mData->mUpdateConn = mData->mStringData->commonData->onStringDataModified.connect(boost::bind(&HString::StringData::updateString, mData.get()));
	}

	HString::HString(const WString& identifierString)
	{
		mData = cm_shared_ptr<StringData>();

		mData->mStringData = &StringTable::instance().getStringData(identifierString);

		if(mData->mStringData->numParameters > 0)
			mData->mParameters = cm_newN<WString>(mData->mStringData->numParameters);

		mData->mUpdateConn = mData->mStringData->commonData->onStringDataModified.connect(boost::bind(&HString::StringData::updateString, mData.get()));
	}

	HString::HString(const HString& copy)
	{
		mData = copy.mData;
	}

	HString::~HString()
	{

	}

	HString::operator const WString& () const 
	{ 
		if(mData->mIsDirty)
		{
			if(mData->mParameters != nullptr)
			{
				mData->mStringData->concatenateString(mData->mCachedString, mData->mParameters, mData->mStringData->numParameters);
				mData->mStringPtr = &mData->mCachedString;
			}
			else
			{
				mData->mStringPtr = &mData->mStringData->string;
			}

			mData->mIsDirty = false;
		}

		return *mData->mStringPtr; 
	}

	void HString::setParameter(UINT32 idx, const WString& value)
	{
		mData->mParameters[idx] = value;

		mData->mIsDirty = true;
		mData->onStringModified();
	}

	boost::signals::connection HString::addOnStringModifiedCallback(boost::function<void()> callback) const
	{
		return mData->onStringModified.connect(callback);
	}
}