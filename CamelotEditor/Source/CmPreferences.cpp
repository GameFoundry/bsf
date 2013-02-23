#include "CmPreferences.h"
#include "CmException.h"
#include "CmDebug.h"
#include "CmFileSystem.h"
#include "CmDataStream.h"

namespace CamelotEditor
{
	void Preferences::setValue(const String& key, const String& value, UINT32 arrayIdx)
	{
		if(!isKeyValid(key))
			CM_EXCEPT(InvalidParametersException, "Invalid key name. Please don't use any special character in the key name: " + key);

		auto findIter = mKeys.find(key);

		if(findIter == mKeys.end() || findIter->second.mapType != VMT_String)
		{
			KeyMapIndex kmi;
			kmi.mapType = VMT_String;
			kmi.entryIdx = (UINT32)mStringEntries.size();
			mStringEntries.push_back(PrefStringEntries());

			mKeys[key] = kmi;

			findIter = mKeys.find(key);
		}

		PrefStringEntries& strEntry = mStringEntries[findIter->second.entryIdx];
		if(strEntry.entries.size() <= arrayIdx)
			strEntry.entries.resize(arrayIdx + 1);

		strEntry.entries[arrayIdx] = value;
	}

	void Preferences::addValue(const String& key, const String& value)
	{
		UINT32 arrayIdx = getNumValues(key);
		setValue(key, value, arrayIdx);
	}

	const String& Preferences::getValue(const String& key, const String& defaultValue, UINT32 arrayIdx)
	{
		auto findIter = mKeys.find(key);

		if(findIter == mKeys.end() || findIter->second.mapType != VMT_String)
			return defaultValue;

		PrefStringEntries& entry = mStringEntries[findIter->second.entryIdx];
		if(arrayIdx < entry.entries.size())
			return entry.entries[arrayIdx];

		return defaultValue;
	}

	void Preferences::setValue(const String& key, const int value, UINT32 arrayIdx)
	{
		if(!isKeyValid(key))
			CM_EXCEPT(InvalidParametersException, "Invalid key name. Please don't use any special character in the key name: " + key);

		auto findIter = mKeys.find(key);

		if(findIter == mKeys.end() || findIter->second.mapType != VMT_Int)
		{
			KeyMapIndex kmi;
			kmi.mapType = VMT_Int;
			kmi.entryIdx = (UINT32)mIntEntries.size();
			mIntEntries.push_back(PrefIntEntries());

			mKeys[key] = kmi;

			findIter = mKeys.find(key);
		}

		PrefIntEntries& intEntry = mIntEntries[findIter->second.entryIdx];
		if(intEntry.entries.size() <= arrayIdx)
			intEntry.entries.resize(arrayIdx + 1);

		intEntry.entries[arrayIdx] = value;
	}

	void Preferences::addValue(const String& key, const int value)
	{
		UINT32 arrayIdx = getNumValues(key);
		setValue(key, value, arrayIdx);
	}

	int Preferences::getValue(const String& key, int defaultValue, UINT32 arrayIdx)
	{
		auto findIter = mKeys.find(key);

		if(findIter == mKeys.end() || findIter->second.mapType != VMT_Int)
			return defaultValue;

		PrefIntEntries& entry = mIntEntries[findIter->second.entryIdx];
		if(arrayIdx < entry.entries.size())
			return entry.entries[arrayIdx];

		return defaultValue;
	}

	void Preferences::setValue(const String& key, float value, UINT32 arrayIdx)
	{
		if(!isKeyValid(key))
			CM_EXCEPT(InvalidParametersException, "Invalid key name. Please don't use any special character in the key name: " + key);

		auto findIter = mKeys.find(key);

		if(findIter == mKeys.end() || findIter->second.mapType != VMT_Float)
		{
			KeyMapIndex kmi;
			kmi.mapType = VMT_Float;
			kmi.entryIdx = (UINT32)mFloatEntries.size();
			mFloatEntries.push_back(PrefFloatEntries());

			mKeys[key] = kmi;

			findIter = mKeys.find(key);
		}

		PrefFloatEntries& floatEntry = mFloatEntries[findIter->second.entryIdx];
		if(floatEntry.entries.size() <= arrayIdx)
			floatEntry.entries.resize(arrayIdx + 1);

		floatEntry.entries[arrayIdx] = value;
	}

	void Preferences::addValue(const String& key, float value)
	{
		UINT32 arrayIdx = getNumValues(key);
		setValue(key, value, arrayIdx);
	}

	float Preferences::getValue(const String& key, float defaultValue, UINT32 arrayIdx)
	{
		auto findIter = mKeys.find(key);

		if(findIter == mKeys.end() || findIter->second.mapType != VMT_Float)
			return defaultValue;

		PrefFloatEntries& entry = mFloatEntries[findIter->second.entryIdx];
		if(arrayIdx < entry.entries.size())
			return entry.entries[arrayIdx];
		
		return defaultValue;
	}

	UINT32 Preferences::getNumValues(const String& key)
	{
		auto findIter = mKeys.find(key);

		if(findIter == mKeys.end())
			return 0;

		switch(findIter->second.mapType)
		{
		case VMT_String:
			return (UINT32)mStringEntries[findIter->second.entryIdx].entries.size();
		case VMT_Float:
			return (UINT32)mFloatEntries[findIter->second.entryIdx].entries.size();
		case VMT_Int:
			return (UINT32)mIntEntries[findIter->second.entryIdx].entries.size();
		}

		return 0;
	}

	bool Preferences::isKeyValid(const String& key) const
	{
		String::size_type foundComma = key.find(",");

		return foundComma == String::npos;
	}

	void Preferences::save(const String& path, bool overwrite) const
	{
		if(FileSystem::fileExists(path))
		{
			if(!overwrite)
			{
				CM_EXCEPT(FileNotFoundException, "File already exists at this location.");
			}
			else
				FileSystem::remove(path);
		}

		map<String, vector<EntryToSave>::type>::type groups;
		for(auto iter = mKeys.begin(); iter != mKeys.end(); ++iter)
		{
			vector<String>::type splitStr = StringUtil::split(iter->first, ".", 1);

			String groupName;
			String paramName;
			if(splitStr.size() > 1)
			{
				groupName = splitStr[0];
				paramName = splitStr[1];
			}
			else
			{
				groupName = "Global";
				paramName = splitStr[0];
			}

			switch(iter->second.mapType)
			{
			case VMT_String:
				{
					const PrefStringEntries& entry = mStringEntries[iter->second.entryIdx];

					if(entry.entries.size() > 1)
					{
						for(size_t i = 0; i < entry.entries.size(); i++)
						{
							EntryToSave newEntry;
							newEntry.name = paramName + "," + toString(i);
							newEntry.value = "\"" + entry.entries[i] + "\"";

							groups[groupName].push_back(newEntry);
						}
					}
					else
					{
						EntryToSave newEntry;
						newEntry.name = paramName;
						newEntry.value = "\"" + entry.entries[0] + "\"";

						groups[groupName].push_back(newEntry);
					}
				}
				break;
			case VMT_Float:
				{
					const PrefFloatEntries& entry = mFloatEntries[iter->second.entryIdx];

					if(entry.entries.size() > 1)
					{
						for(size_t i = 0; i < entry.entries.size(); i++)
						{
							EntryToSave newEntry;
							newEntry.name = paramName + "," + toString(i);
							newEntry.value = toString(entry.entries[i], 12) + "f";

							groups[groupName].push_back(newEntry);
						}
					}
					else
					{
						EntryToSave newEntry;
						newEntry.name = paramName;
						newEntry.value = toString(entry.entries[0], 12) + "f";

						groups[groupName].push_back(newEntry);
					}
				}
				break;
			case VMT_Int:
				{
					const PrefIntEntries& entry = mIntEntries[iter->second.entryIdx];

					if(entry.entries.size() > 1)
					{
						for(size_t i = 0; i < entry.entries.size(); i++)
						{
							EntryToSave newEntry;
							newEntry.name = paramName + "," + toString(i);
							newEntry.value = toString(entry.entries[i]);

							groups[groupName].push_back(newEntry);
						}
					}
					else
					{
						EntryToSave newEntry;
						newEntry.name = paramName;
						newEntry.value = toString(entry.entries[0]);

						groups[groupName].push_back(newEntry);
					}
				}
				break;
			}
		}

		stringstream strStream;
		for(auto iter = groups.begin(); iter != groups.end(); ++iter)
		{
			strStream<<"["<<iter->first<<"]\r\n";

			for(auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2)
			{
				strStream<<iter2->name<<" = "<<iter2->value<<"\r\n";
			}

			strStream<<"\r\n";
		}

		String outputString = strStream.str();

		DataStreamPtr stream = FileSystem::create(path);
		stream->write(outputString.data(), outputString.size());
		stream->close();
	}

	void Preferences::load(const String& path)
	{
		if(!FileSystem::fileExists(path))
			CM_EXCEPT(FileNotFoundException, "Specified file: " + path + " does not exist.");

		DataStreamPtr stream = FileSystem::open(path);
		char buffer[4096];

		String currentGroupName = "Global";
		bool groupIsDefault = true;
		int foundGroups = 0;
		while(!stream->eof())
		{
			stream->readLine(buffer, sizeof(buffer));
			String line(buffer);

			String::size_type bracketsStart = line.find("[");
			String::size_type bracketsEnd = line.find("]");
			if (bracketsStart != String::npos && bracketsEnd != String::npos)
			{
				currentGroupName = line.substr(bracketsStart + 1, bracketsEnd - (bracketsStart + 1));
				groupIsDefault = foundGroups == 0;
				foundGroups++;
				continue;
			}

			vector<String>::type paramSplit = StringUtil::split(line, "=");
			if(paramSplit.size() < 2)
			{
				LOGWRN("Cannot parse line in file " + path + ". Invalid number of elements: " + line);
				continue;
			}

			String paramName = paramSplit[0];
			StringUtil::trim(paramName);

			vector<String>::type arrayElemSplit = StringUtil::split(paramName, ",");
			UINT32 arrayIdx = 0;
			if(arrayElemSplit.size() > 1)
			{
				String arrayElemStr = arrayElemSplit[1];
				StringUtil::trim(arrayElemStr);
				arrayIdx = parseInt(arrayElemStr);

				paramName = arrayElemSplit[0];
				StringUtil::trim(paramName);
			}

			if(!groupIsDefault)
				paramName = currentGroupName + "." + paramName;

			String paramValue = paramSplit[1];
			StringUtil::trim(paramValue);

			// Float
			if(StringUtil::endsWith(paramValue, "f"))
			{
				paramValue.erase(paramValue.begin() + paramValue.size() - 1);

				float fltValue = parseFloat(paramValue);
				setValue(paramName, fltValue, arrayIdx);
			}
			else if(StringUtil::endsWith(paramValue, "\"")) // String
			{
				paramValue.erase(paramValue.begin());
				paramValue.erase(paramValue.begin() + paramValue.size() - 1);

				setValue(paramName, paramValue, arrayIdx);
			}
			else // Assuming int
			{
				int intValue = parseInt(paramValue);
				setValue(paramName, intValue, arrayIdx);
			}
		}

		stream->close();
	}
}