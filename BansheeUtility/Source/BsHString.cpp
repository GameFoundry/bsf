//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsPrerequisitesUtil.h"
#include "BsHString.h"
#include "BsStringTable.h"

namespace BansheeEngine
{
	HString::StringData::StringData()
		:mParameters(nullptr), mIsDirty(true), mStringPtr(nullptr)
	{ }

	HString::StringData::~StringData()
	{
		mUpdateConn.disconnect();

		if(mParameters != nullptr)
			bs_deleteN(mParameters, mStringData->numParameters);
	}

	void HString::StringData::updateString()
	{
		LocalizedStringData* stringData = &StringTable::instance().getStringData(mStringData->commonData->identifier);

		// If common data changed re-apply the connections
		if(stringData->commonData != mStringData->commonData)
		{
			mUpdateConn.disconnect();
			mUpdateConn = stringData->commonData->onStringDataModified.connect(std::bind(&HString::StringData::updateString, this));
		}

		mStringData = stringData;
		mIsDirty = true;

		onStringModified();
	}

	HString::HString()
	{
		mData = bs_shared_ptr<StringData>();

		mData->mStringData = &StringTable::instance().getStringData(L"");

		if(mData->mStringData->numParameters > 0)
			mData->mParameters = bs_newN<WString>(mData->mStringData->numParameters);

		mData->mUpdateConn = mData->mStringData->commonData->onStringDataModified.connect(std::bind(&HString::StringData::updateString, mData.get()));
	}

	HString::HString(const WString& identifierString)
	{
		mData = bs_shared_ptr<StringData>();

		mData->mStringData = &StringTable::instance().getStringData(identifierString);

		if(mData->mStringData->numParameters > 0)
			mData->mParameters = bs_newN<WString>(mData->mStringData->numParameters);

		mData->mUpdateConn = mData->mStringData->commonData->onStringDataModified.connect(std::bind(&HString::StringData::updateString, mData.get()));
	}

	HString::HString(const WString& identifierString, const WString& defaultString)
	{
		mData = bs_shared_ptr<StringData>();

		StringTable::instance().setString(identifierString, StringTable::DEFAULT_LANGUAGE, defaultString);

		mData->mStringData = &StringTable::instance().getStringData(identifierString);

		if (mData->mStringData->numParameters > 0)
			mData->mParameters = bs_newN<WString>(mData->mStringData->numParameters);

		mData->mUpdateConn = mData->mStringData->commonData->onStringDataModified.connect(std::bind(&HString::StringData::updateString, mData.get()));
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
		return getValue(); 
	}

	const WString& HString::getValue() const
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

	HEvent HString::addOnStringModifiedCallback(std::function<void()> callback) const
	{
		return mData->onStringModified.connect(callback);
	}

	const HString& HString::dummy()
	{
		static HString dummyVal;

		return dummyVal;
	}
}