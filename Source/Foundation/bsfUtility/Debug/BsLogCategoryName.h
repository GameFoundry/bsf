//************************************ bs::framework - Copyright 2019 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs
{
	/** @addtogroup Debug
	 *  @{
	 */
	
	/** Manages the translation of the Category type to string based on its ID */
	class BS_UTILITY_EXPORT LogCategoryName
	{
	public:
		LogCategoryName() = default;
		
		/**
		 *  Registers the new category as a pair of its ID and name as a String
		 * 
		 *  @param[in] id    Number representing the Category's ID.
		 *  @param[in] name  Name of the Category.
		 *  @return False if the ID is already taken, otherwise true.
		 */
		bool registerCategory(UINT32 id, String name);
		
		/**
		 *  Unregisters the category based on its ID.
		 *  
		 *  @param[in] id   Number representing the Category's ID.
		 */
		void unregisterCategory(UINT32 id);
		
		/**
		 * Checks, if the ID has the registered name.  
		 *
		 *  @param[in] id    Number representing the Category's ID.
		 *  @return true if exists, otherwise false.
		 */
		bool categoryExist(UINT32 id);
		
		/** Returns the number of registered log categories. */
		UINT32 getNumRegisteredLogCategories() const { return mNumCategories; };
		
		/**
		 *  Get the name of the category based on its ID.
		 *  
		 *  @param[in]	id	Number representing the Category's ID.
		 *  @param[in]	name	Variable, in which the name will be written.
		 *  @return If found will write the name and return true. Otherwise will write the name "Unknown" and return false.
		 */
		bool getCategoryName(UINT32 id, String& name);
		
	private:
		friend class Debug;
	
		UINT32 mNumCategories = 0;
		Map<UINT32, String> mIdStringPairs;
		mutable RecursiveMutex mMutex;
	};
	
	/** A simple way to access the Log Category Name module. */
	BS_UTILITY_EXPORT LogCategoryName& gLogCategoryName();
	
	/** 
	 *  Convenient macro that registers the Category based on its name. 
	 *  
	 *  @note Remember to create the log category using the BS_LOG_CATEGORY() macro.
	 */
	#define BS_LOG_REGISTER_CATEGORY_NAME(category) gLogCategoryName().registerCategory(LogCategory##category::_id, #category);
	
	/** @} */
}