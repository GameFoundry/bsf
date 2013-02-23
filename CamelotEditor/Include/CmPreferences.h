#pragma once

#include "CmEditorPrerequisites.h"

namespace CamelotEditor
{
	class Preferences
	{
	public:
		void setValue(const String& key, const String& value, CamelotEngine::UINT32 arrayIdx = 0);
		void addValue(const String& key, const String& value);
		const String& getValue(const String& key, const String& defaultValue = StringUtil::BLANK, CamelotEngine::UINT32 arrayIdx = 0);

		void setValue(const String& key, const int value, CamelotEngine::UINT32 arrayIdx = 0);
		void addValue(const String& key, const int value);
		int getValue(const String& key, int defaultValue = 0, CamelotEngine::UINT32 arrayIdx = 0);

		void setValue(const String& key, float value, CamelotEngine::UINT32 arrayIdx = 0);
		void addValue(const String& key, float value);
		float getValue(const String& key, float defaultValue = 0.0f, CamelotEngine::UINT32 arrayIdx = 0);

		CamelotEngine::UINT32 getNumValues(const String& key);

		void save(const String& path, bool overwrite = true) const;
		void load(const String& path);

	private:
		struct PrefStringEntries
		{
			vector<String>::type entries;
		};

		struct PrefIntEntries
		{
			vector<int>::type entries;
		};

		struct PrefFloatEntries
		{
			vector<float>::type entries;
		};

		enum ValueMapType
		{
			VMT_String,
			VMT_Int,
			VMT_Float
		};

		struct KeyMapIndex
		{
			ValueMapType mapType;
			CamelotEngine::UINT32 entryIdx;
		};

		struct EntryToSave
		{
			String name;
			String value;
		};

		map<String, KeyMapIndex>::type	mKeys;
		vector<PrefStringEntries>::type mStringEntries;
		vector<PrefIntEntries>::type	mIntEntries;
		vector<PrefFloatEntries>::type	mFloatEntries;

		bool isKeyValid(const String& key) const;
	};
}