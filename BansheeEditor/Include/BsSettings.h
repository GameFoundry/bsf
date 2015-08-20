#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains a serializable set of generic key-value pairs.
	 */
	class BS_ED_EXPORT Settings : public IReflectable
	{
	public:
		Settings();

		/**
		 * @brief	Adds or updates a property key/value pair with a floating point value.
		 */
		void setFloat(const String& name, float value);

		/**
		 * @brief	Adds or updates a property key/value pair with a signed integer value.
		 */
		void setInt(const String& name, INT32 value);

		/**
		 * @brief	Adds or updates a property key/value pair with a boolean value.
		 */
		void setBool(const String& name, bool value);

		/**
		 * @brief	Adds or updates a property key/value pair with a string value.
		 */
		void setString(const String& name, const WString& value);

		/**
		 * @brief	Returns the floating point value of the specified key, or the default value
		 *			if such key cannot be found.
		 */
		float getFloat(const String& name, float defaultValue = 0.0f);

		/**
		 * @brief	Returns the integer point value of the specified key, or the default value
		 *			if such key cannot be found.
		 */
		INT32 getInt(const String& name, INT32 defaultValue = 0);

		/**
		 * @brief	Returns the boolean point value of the specified key, or the default value
		 *			if such key cannot be found.
		 */
		bool getBool(const String& name, bool defaultValue = false);

		/**
		 * @brief	Returns the string point value of the specified key, or the default value
		 *			if such key cannot be found.
		 */
		WString getString(const String& name, const WString& defaultValue = StringUtil::WBLANK);

		/**
		 * @brief	Returns true if the key with the specified name exists.
		 */
		bool hasKey(const String& name);

		/**
		 * @brief	Deletes a key with the specified name.
		 */
		void deleteKey(const String& name);

		/**
		 * @brief	Deletes all key/value pairs.
		 */
		void deleteAllKeys();

		/**
		 * @brief	Returns a hash value that may be used for checking if any internal settings were
		 *			modified.
		 */
		UINT32 getHash() const { return mHash; }

	protected:
		/**
		 * @brief	Marks the object as dirty so that outside objects know when to update.
		 */
		void markAsDirty() const { mHash++; }

		Map<String, float> mFloatProperties;
		Map<String, INT32> mIntProperties;
		Map<String, bool> mBoolProperties;
		Map<String, WString> mStringProperties;

		mutable UINT32 mHash;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class SettingsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
	};
}