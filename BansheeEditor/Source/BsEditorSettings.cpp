#include "BsEditorSettings.h"

namespace BansheeEngine
{
	EditorSettings::EditorSettings()
		:mMoveSnapActive(false), mRotateSnapActive(false), mMoveSnap(0.1f), mRotationSnap(20.0f),
		mGridSize(256), mGridAxisSpacing(1.0f), mHandleSize(0.10f), mHash(0), mActiveSceneTool(1 /* Move */),
		mActiveCoordinateMode(0), mActivePivotMode(0)
	{ }

	void EditorSettings::setFloat(const String& name, float value)
	{
		mFloatProperties[name] = value;
		mIntProperties.erase(name);
		mBoolProperties.erase(name);
		mStringProperties.erase(name);

		markAsDirty();
	}

	void EditorSettings::setInt(const String& name, INT32 value)
	{
		mFloatProperties.erase(name);
		mIntProperties[name] = value;
		mBoolProperties.erase(name);
		mStringProperties.erase(name);

		markAsDirty();
	}

	void EditorSettings::setBool(const String& name, bool value)
	{
		mFloatProperties.erase(name);
		mIntProperties.erase(name);
		mBoolProperties[name] = value; 
		mStringProperties.erase(name);

		markAsDirty();
	}

	void EditorSettings::setString(const String& name, const WString& value)
	{
		mFloatProperties.erase(name);
		mIntProperties.erase(name);
		mBoolProperties.erase(name);
		mStringProperties[name] = value;

		markAsDirty();
	}

	float EditorSettings::getFloat(const String& name, float defaultValue)
	{
		auto iterFind = mFloatProperties.find(name);
		if (iterFind != mFloatProperties.end())
			return iterFind->second;

		return defaultValue;
	}

	INT32 EditorSettings::getInt(const String& name, INT32 defaultValue)
	{
		auto iterFind = mIntProperties.find(name);
		if (iterFind != mIntProperties.end())
			return iterFind->second;

		return defaultValue;
	}

	bool EditorSettings::getBool(const String& name, bool defaultValue)
	{
		auto iterFind = mBoolProperties.find(name);
		if (iterFind != mBoolProperties.end())
			return iterFind->second;

		return defaultValue;
	}

	WString EditorSettings::getString(const String& name, const WString& defaultValue)
	{
		auto iterFind = mStringProperties.find(name);
		if (iterFind != mStringProperties.end())
			return iterFind->second;

		return defaultValue;
	}

	bool EditorSettings::hasKey(const String& name)
	{
		if (mFloatProperties.find(name) != mFloatProperties.end())
			return true;
		
		if (mIntProperties.find(name) != mIntProperties.end())
			return true;

		if (mBoolProperties.find(name) != mBoolProperties.end())
			return true;

		if (mStringProperties.find(name) != mStringProperties.end())
			return true;

		return false;
	}

	void EditorSettings::deleteKey(const String& name)
	{
		mFloatProperties.erase(name);
		mIntProperties.erase(name);
		mBoolProperties.erase(name);
		mStringProperties.erase(name);

		markAsDirty();
	}

	void EditorSettings::deleteAllKeys()
	{
		mFloatProperties.clear();
		mIntProperties.clear();
		mBoolProperties.clear();
		mStringProperties.clear();

		markAsDirty();
	}
}