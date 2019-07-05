//************************************ bs::framework - Copyright 2019 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Debug/BsLogCategoryName.h"

namespace bs
{
	bool LogCategoryName::registerCategory(UINT32 id, String name)
	{
		RecursiveLock lock(mMutex);
		
		if (!categoryExist (id))
		{
			mIdStringPairs.emplace(id, name);
			mNumCategories++;
			return true;
		}
		return false;
	}
	
	void LogCategoryName::unregisterCategory(UINT32 id)
	{
		RecursiveLock lock(mMutex);
		
		if (categoryExist (id))
		{
			mIdStringPairs.erase(id);
			mNumCategories--;
		}
	}
	
	bool LogCategoryName::categoryExist(UINT32 id)
	{
		return mIdStringPairs.find(id) != mIdStringPairs.end();
	}
	
	bool LogCategoryName::getCategoryName(UINT32 id, String& name)
	{
		RecursiveLock lock(mMutex);
		
		if (categoryExist(id))
		{
			auto search = mIdStringPairs.find(id);
			name = search->second;
			return true;
		}
		else
		{
			name = "Unknown";
			return false;
		}
	}
	
	BS_UTILITY_EXPORT LogCategoryName& gLogCategoryName()
	{
		static LogCategoryName logCategoryName;
		return logCategoryName;
	}
}