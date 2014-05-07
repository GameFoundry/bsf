#include "CmStringTable.h"
#include "CmException.h"

namespace BansheeEngine
{
	const Language StringTable::DEFAULT_LANGUAGE = Language::EnglishUS;

	LocalizedStringData::LocalizedStringData()
		:commonData(nullptr), parameterOffsets(nullptr), numParameters(0)
	{

	}

	LocalizedStringData::~LocalizedStringData()
	{
		if(parameterOffsets != nullptr)
			cm_deleteN(parameterOffsets, numParameters);
	}

	void LocalizedStringData::concatenateString(WString& outputString, WString* parameters, UINT32 numParameterValues) const
	{
		// A safeguard in case translated strings have different number of parameters
		UINT32 actualNumParameters = std::min(numParameterValues, numParameters);
		
		if(parameters != nullptr)
		{
			UINT32 totalNumChars = 0;
			UINT32 prevIdx = 0;
			for(UINT32 i = 0; i < actualNumParameters; i++)
			{
				totalNumChars += (parameterOffsets[i].location - prevIdx) + (UINT32)parameters[parameterOffsets[i].paramIdx].size();;

				prevIdx = parameterOffsets[i].location;
			}

			totalNumChars += (UINT32)string.size() - prevIdx;

			outputString.resize(totalNumChars);
			wchar_t* strData = &outputString[0]; // String contiguity required by C++11, but this should work elsewhere as well

			prevIdx = 0;
			for(UINT32 i = 0; i < actualNumParameters; i++)
			{
				UINT32 strSize = parameterOffsets[i].location - prevIdx;
				memcpy(strData, &string[prevIdx], strSize * sizeof(wchar_t));
				strData += strSize;

				WString& param = parameters[parameterOffsets[i].paramIdx];
				memcpy(strData, &param[0], param.size() * sizeof(wchar_t));
				strData += param.size();

				prevIdx = parameterOffsets[i].location;
			}

			memcpy(strData, &string[prevIdx], (string.size() - prevIdx) * sizeof(wchar_t));
		}
		else
		{
			outputString.resize(string.size());
			wchar_t* strData = &outputString[0]; // String contiguity required by C++11, but this should work elsewhere as well

			memcpy(strData, &string[0], string.size() * sizeof(wchar_t));
		}
	}

	void LocalizedStringData::updateString(const WString& _string)
	{
		if(parameterOffsets != nullptr)
			cm_deleteN(parameterOffsets, numParameters);

		Vector<ParamOffset> paramOffsets;

		INT32 lastBracket = -1;
		WStringStream bracketChars;
		WStringStream cleanString;
		bool escaped = false;
		UINT32 numRemovedChars = 0;
		for(UINT32 i = 0; i < (UINT32)_string.size(); i++)
		{
			if(_string[i] == '\\' && !escaped)
			{
				numRemovedChars++;
				escaped = true;
				continue;
			}

			if(lastBracket == -1)
			{
				// If current char is non-escaped opening bracket start parameter definition
				if(_string[i] == '{' && !escaped) 
					lastBracket = i;
				else
					cleanString<<_string[i];
			}
			else
			{
				if(isdigit(_string[i]))
					bracketChars<<_string[i];
				else
				{
					// If current char is non-escaped closing bracket end parameter definition
					UINT32 numParamChars = (UINT32)bracketChars.tellp();
					if(_string[i] == '}' && numParamChars > 0 && !escaped) 
					{
						numRemovedChars += numParamChars + 2; // +2 for open and closed brackets

						UINT32 paramIdx = parseUnsignedInt(bracketChars.str());
						paramOffsets.push_back(ParamOffset(paramIdx, i + 1 - numRemovedChars));
					}
					else
					{
						// Last bracket wasn't really a parameter
						for(UINT32 j = lastBracket; j <= i; j++)
							cleanString<<_string[j];
					}

					lastBracket = -1;
					bracketChars.clear();
				}
			}

			escaped = false;
		}

		string = cleanString.str();
		numParameters = (UINT32)paramOffsets.size();

		// Try to find out of order param offsets and fix them
		std::sort(begin(paramOffsets), end(paramOffsets), 
			[&] (const ParamOffset& a, const ParamOffset& b) { return a.paramIdx < b.paramIdx; } );

		if(paramOffsets.size() > 0)
		{
			UINT32 sequentialIdx = 0;
			UINT32 lastParamIdx = paramOffsets[0].paramIdx;
			for(UINT32 i = 0; i < numParameters; i++)
			{
				if(paramOffsets[i].paramIdx == lastParamIdx)
				{
					paramOffsets[i].paramIdx = sequentialIdx;
					continue;
				}

				lastParamIdx = paramOffsets[i].paramIdx;
				sequentialIdx++;

				paramOffsets[i].paramIdx = sequentialIdx;
			}
		}

		// Re-sort based on location since we find that more useful at runtime
		std::sort(begin(paramOffsets), end(paramOffsets), 
			[&] (const ParamOffset& a, const ParamOffset& b) { return a.location < b.location; } );

		parameterOffsets = cm_newN<ParamOffset>(numParameters);
		for(UINT32 i = 0; i < numParameters; i++)
			parameterOffsets[i] = paramOffsets[i];
	}

	StringTable::StringTable()
		:mActiveLanguageData(nullptr), mDefaultLanguageData(nullptr), mAllLanguages(nullptr)
	{
		mAllLanguages = cm_newN<LanguageData>((UINT32)Language::Count);

		mDefaultLanguageData = &(mAllLanguages[(UINT32)DEFAULT_LANGUAGE]);
		mActiveLanguageData = mDefaultLanguageData;
		mActiveLanguage = DEFAULT_LANGUAGE;
	}
	
	StringTable::~StringTable()
	{
		for(UINT32 i = 0; i < (UINT32)Language::Count; i++)
		{
			for(auto& iter : mAllLanguages[i].strings)
				cm_delete(iter.second);
		}

		cm_deleteN(mAllLanguages, (UINT32)Language::Count);

		for(auto& common : mCommonData)
			cm_delete(common.second);
	}

	void StringTable::setActiveLanguage(Language language)
	{
		if(language == mActiveLanguage)
			return;

		mActiveLanguageData = &(mAllLanguages[(UINT32)language]);
		mActiveLanguage = language;

		notifyAllStringsChanged();
	}

	void StringTable::setString(const WString& identifier, Language language, const WString& string)
	{
		LanguageData* curLanguage = &(mAllLanguages[(UINT32)language]);

		auto iterFind = curLanguage->strings.find(identifier);

		LocalizedStringData* stringData;
		if(iterFind == curLanguage->strings.end())
		{
			auto iterFindCommon = mCommonData.find(identifier);

			LocalizedStringData::Common* common = nullptr;
			if(iterFindCommon == mCommonData.end())
			{
				common = cm_new<LocalizedStringData::Common>();
				common->identifier = identifier;
				mCommonData[identifier] = common;
			}
			else
				common = iterFindCommon->second;

			stringData = cm_new<LocalizedStringData>();
			curLanguage->strings[identifier] = stringData;
			stringData->commonData = common;
		}
		else
		{
			stringData = iterFind->second;
		}

		stringData->updateString(string);

		if(mActiveLanguage == language)
		{
			if(!stringData->commonData->onStringDataModified.empty())
				stringData->commonData->onStringDataModified();
		}
	}

	void StringTable::removeString(const WString& identifier)
	{
		// Order of operations is very important here, in case a string that is in use
		// is removed. In that case we want the string to be marked as modified and it should
		// call getStringData which will generate a new entry for the string.
		
		LocalizedStringData* stringData = nullptr;
		for(UINT32 i = 0; i < (UINT32)Language::Count; i++)
		{
			auto findIter = mAllLanguages[i].strings.find(identifier);
			if(findIter != mAllLanguages[i].strings.end())
			{
				if(mActiveLanguage == (Language)i)
					stringData = findIter->second;
				else
					cm_delete(findIter->second);

				mAllLanguages[i].strings.erase(findIter);
			}
		}

		auto findIterCommon = mCommonData.find(identifier);

		LocalizedStringData::Common* common = nullptr;
		if(findIterCommon != mCommonData.end())
		{
			common = findIterCommon->second;
			mCommonData.erase(findIterCommon);

			if(!common->onStringDataModified.empty())
				common->onStringDataModified();
		}

		if(stringData != nullptr)
			cm_delete(stringData);

		if(common != nullptr)
			cm_delete(common);
	}

	LocalizedStringData& StringTable::getStringData(const WString& identifier, bool insertIfNonExisting)
	{
		return getStringData(identifier, mActiveLanguage, insertIfNonExisting);
	}

	LocalizedStringData& StringTable::getStringData(const WString& identifier, Language language, bool insertIfNonExisting)
	{
		LanguageData* curLanguage = &(mAllLanguages[(UINT32)language]);

		auto iterFind = curLanguage->strings.find(identifier);
		if(iterFind != curLanguage->strings.end())
			return *iterFind->second;

		auto defaultIterFind = mDefaultLanguageData->strings.find(identifier);
		if(defaultIterFind != mDefaultLanguageData->strings.end())
			return *defaultIterFind->second;

		if(insertIfNonExisting)
		{
			setString(identifier, DEFAULT_LANGUAGE, identifier);

			auto defaultIterFind = mDefaultLanguageData->strings.find(identifier);
			if(defaultIterFind != mDefaultLanguageData->strings.end())
				return *defaultIterFind->second;
		}

		CM_EXCEPT(InvalidParametersException, "There is no string data for the provided identifier.");
	}

	void StringTable::notifyAllStringsChanged()
	{
		for(auto& iter : mCommonData)
		{
			if(!iter.second->onStringDataModified.empty())
				iter.second->onStringDataModified();
		}
	}
}