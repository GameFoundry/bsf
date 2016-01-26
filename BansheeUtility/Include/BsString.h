#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Basic string that uses Banshee memory allocators.
	 */
	template <typename T>
	using BasicString = std::basic_string<T, std::char_traits<T>, StdAlloc<T>>;

	/**
	 * @brief	Basic string stream that uses Banshee memory allocators.
	 */
	template <typename T>
	using BasicStringStream = std::basic_stringstream<T, std::char_traits<T>, StdAlloc<T>>;

	/**
	 * @brief	Wide string used primarily for handling Unicode text.
	 */
	typedef BasicString<wchar_t> WString;

	/**
	 * @brief	Narrow string used primarily for handling ASCII text.
	 */
	typedef BasicString<char> String;

	/**
	 * @brief	Wide string stream used for primarily for constructing
	 * 			strings consisting of Unicode text.
	 */
	typedef BasicStringStream<wchar_t> WStringStream;

	/**
	 * @brief	Wide string stream used for primarily for constructing
	 * 			strings consisting of ASCII text.
	 */
	typedef BasicStringStream<char> StringStream;

    /**
     * @brief	Utility class for manipulating Strings.
     */
    class BS_UTILITY_EXPORT StringUtil
    {
	public:
        /**
         * @brief	Removes any whitespace characters from beginning or end of the string.
         */
        static void trim(String& str, bool left = true, bool right = true);

        /**
         * @copydoc StringUtil::trim(String&, bool, bool)
         */
        static void trim(WString& str, bool left = true, bool right = true);

        /**
         * @brief	Removes specified characters from beginning or end of the string.
         */
        static void trim(String& str, const String& delims, bool left = true, bool right = true);

		/**
         * @copydoc StringUtil::trim(String&, const String&, bool, bool)
         */
        static void trim(WString& str, const WString& delims, bool left = true, bool right = true);

		/**
		 * @brief	Returns a vector of strings containing all the substrings delimited
		 * 			by the provided delimiter characters.
		 *
		 * @param	str		 	The string to split.
		 * @param	delims   	(optional) Delimiter characters to split the string by. They will not
		 * 						be included in resulting substrings.
		 * @param	maxSplits	(optional) The maximum number of splits to perform (0 for unlimited splits). If this
		 *						parameters is > 0, the splitting process will stop after this many splits, left to right.
		 */
		static Vector<String> split(const String& str, const String& delims = "\t\n ", unsigned int maxSplits = 0);

		/**
		 * @copydoc	StringUtil::split(const String&, const String&, unsigned int)
		 */
		static Vector<WString> split(const WString& str, const WString& delims = L"\t\n ", unsigned int maxSplits = 0);

		/**
		 * @brief	Returns a vector of strings containing all the substrings delimited
		 *			by the provided delimiter characters, or the double delimiters used for including
		 *			normal delimiter characters in the tokenized string. 
		 *
		 * @param	str		 	The string to split.
		 * @param	delims   	(optional) Delimiter characters to split the string by. They will not
		 * 						be included in resulting substrings.
		 * @params	doubleDelims (optional) Delimiter character you may use to surround other normal delimiters, in order
		 * 						to include them in the tokensized string.
		 * @param	maxSplits	(optional) The maximum number of splits to perform (0 for unlimited splits). If this
		 *						parameters is > 0, the splitting process will stop after this many splits, left to right.
		 */
		static Vector<String> tokenise(const String& str, const String& delims = "\t\n ", const String& doubleDelims = "\"", unsigned int maxSplits = 0);

		/**
		 * @copydoc	StringUtil::tokenise(const String&, const String&, const String&, unsigned int)
		 */
		static Vector<WString> tokenise(const WString& str, const WString& delims = L"\t\n ", const WString& doubleDelims = L"\"", unsigned int maxSplits = 0);

        /**
         * @brief	Converts all the characters in the string to lower case.
         */
        static void toLowerCase(String& str);

        /**
         * @brief	Converts all the characters in the string to lower case.
         */
        static void toLowerCase(WString& str);

        /**
         * @brief	Converts all the characters in the string to upper case.
         */
        static void toUpperCase(String& str);

        /**
         * @brief	Converts all the characters in the string to upper case.
         */
        static void toUpperCase(WString& str);

        /**
         * @brief	Returns whether the string begins with the pattern passed in.
         *
         * @param	str		 	String to compare.
         * @param	pattern		Pattern to compare with.
		 * @param	lowerCase	(optional) If true, the start of the string will be lower cased before
		 *						comparison, and the pattern should also be in lower case.
         */
        static bool startsWith(const String& str, const String& pattern, bool lowerCase = true);

        /**
         * @copydoc startsWidth(const String&, const String&, bool)
         */
        static bool startsWith(const WString& str, const WString& pattern, bool lowerCase = true);

        /**
         * @brief	Returns whether the string end with the pattern passed in.
         *
         * @param	str		 	String to compare.
         * @param	pattern		Pattern to compare with.
		 * @param	lowerCase	(optional) If true, the start of the string will be lower cased before
		 *						comparison, and the pattern should also be in lower case.
         */
        static bool endsWith(const String& str, const String& pattern, bool lowerCase = true);

        /**
         * @copydoc endsWith(const String&, const String&, bool)
         */
        static bool endsWith(const WString& str, const WString& pattern, bool lowerCase = true);

        /**
         * @brief	Returns true if the string matches the provided pattern. Pattern may use
         * 			a "*" wildcard for matching any characters.
         *
         * @param	str			 	The string to test.
         * @param	pattern		 	Patterns to look for.
         * @param	caseSensitive	(optional) Should the match be case sensitive or not.
         */
        static bool match(const String& str, const String& pattern, bool caseSensitive = true);

		/**
         * @copydoc match(const String&, const String&, bool)
         */
        static bool match(const WString& str, const WString& pattern, bool caseSensitive = true);

		/**
		 * @brief	 Replace all instances of a substring with a another substring.
		 *
		 * @param	source		   	String to search.
		 * @param	replaceWhat	   	Substring to find and replace
		 * @param	replaceWithWhat	Substring to replace with (the new sub-string)
		 *
		 * @return	An updated string with the substrings replaced.
		 */
		static const String replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat);

		/**
         * @copydoc match(const String&, const String&, const String&)
         */
		static const WString replaceAll(const WString& source, const WString& replaceWhat, const WString& replaceWithWhat);

		/**
		 * @brief	Constant blank string, useful for returning by ref where local does not exist.
		 */
        static const String BLANK;

		/**
		 * @brief	Constant blank wide string, useful for returning by ref where local does not exist.
		 */
		static const WString WBLANK;

	private:
		template <class T>
		static typename Vector<typename BasicString<T>> splitInternal(const typename BasicString<T>& str, const typename BasicString<T>& delims, unsigned int maxSplits)
		{
			Vector<BasicString<T>> ret;
			// Pre-allocate some space for performance
			ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

			unsigned int numSplits = 0;

			// Use STL methods 
			size_t start, pos;
			start = 0;
			do 
			{
				pos = str.find_first_of(delims, start);
				if (pos == start)
				{
					// Do nothing
					start = pos + 1;
				}
				else if (pos == BasicString<T>::npos || (maxSplits && numSplits == maxSplits))
				{
					// Copy the rest of the string
					ret.push_back(str.substr(start));
					break;
				}
				else
				{
					// Copy up to delimiter
					ret.push_back(str.substr(start, pos - start));
					start = pos + 1;
				}
				// parse up to next real data
				start = str.find_first_not_of(delims, start);
				++numSplits;

			} while (pos != BasicString<T>::npos);

			return ret;
		}

		template <class T>
		static typename Vector<typename BasicString<T>> tokeniseInternal(const typename BasicString<T>& str, const typename BasicString<T>& singleDelims, 
			const typename BasicString<T>& doubleDelims, unsigned int maxSplits)
		{
			Vector<BasicString<T>> ret;
			// Pre-allocate some space for performance
			ret.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

			unsigned int numSplits = 0;
			BasicString<T> delims = singleDelims + doubleDelims;

			// Use STL methods 
			size_t start, pos;
			T curDoubleDelim = 0;
			start = 0;
			do 
			{
				if (curDoubleDelim != 0)
				{
					pos = str.find(curDoubleDelim, start);
				}
				else
				{
					pos = str.find_first_of(delims, start);
				}

				if (pos == start)
				{
					T curDelim = str.at(pos);
					if (doubleDelims.find_first_of(curDelim) != BasicString<T>::npos)
					{
						curDoubleDelim = curDelim;
					}
					// Do nothing
					start = pos + 1;
				}
				else if (pos == BasicString<T>::npos || (maxSplits && numSplits == maxSplits))
				{
					if (curDoubleDelim != 0)
					{
						//Missing closer. Warn or throw exception?
					}
					// Copy the rest of the string
					ret.push_back( str.substr(start) );
					break;
				}
				else
				{
					if (curDoubleDelim != 0)
					{
						curDoubleDelim = 0;
					}

					// Copy up to delimiter
					ret.push_back( str.substr(start, pos - start) );
					start = pos + 1;
				}
				if (curDoubleDelim == 0)
				{
					// parse up to next real data
					start = str.find_first_not_of(singleDelims, start);
				}

				++numSplits;

			} while (pos != BasicString<T>::npos);

			return ret;
		}

		template <class T>
		static bool startsWithInternal(const typename BasicString<T>& str, const typename BasicString<T>& pattern, bool lowerCase)
		{
			size_t thisLen = str.length();
			size_t patternLen = pattern.length();
			if (thisLen < patternLen || patternLen == 0)
				return false;

			BasicString<T> startOfThis = str.substr(0, patternLen);
			if (lowerCase)
				StringUtil::toLowerCase(startOfThis);

			return (startOfThis == pattern);
		}

		template <class T>
		static bool endsWithInternal(const typename BasicString<T>& str, const typename BasicString<T>& pattern, bool lowerCase)
		{
			size_t thisLen = str.length();
			size_t patternLen = pattern.length();
			if (thisLen < patternLen || patternLen == 0)
				return false;

			BasicString<T> endOfThis = str.substr(thisLen - patternLen, patternLen);
			if (lowerCase)
				StringUtil::toLowerCase(endOfThis);

			return (endOfThis == pattern);
		}

		template <class T>
		static bool matchInternal(const typename BasicString<T>& str, const typename BasicString<T>& pattern, bool caseSensitive)
		{
			BasicString<T> tmpStr = str;
			BasicString<T> tmpPattern = pattern;
			if (!caseSensitive)
			{
				StringUtil::toLowerCase(tmpStr);
				StringUtil::toLowerCase(tmpPattern);
			}

			BasicString<T>::const_iterator strIt = tmpStr.begin();
			BasicString<T>::const_iterator patIt = tmpPattern.begin();
			BasicString<T>::const_iterator lastWildCardIt = tmpPattern.end();
			while (strIt != tmpStr.end() && patIt != tmpPattern.end())
			{
				if (*patIt == '*')
				{
					lastWildCardIt = patIt;
					// Skip over looking for next character
					++patIt;
					if (patIt == tmpPattern.end())
					{
						// Skip right to the end since * matches the entire rest of the string
						strIt = tmpStr.end();
					}
					else
					{
						// scan until we find next pattern character
						while(strIt != tmpStr.end() && *strIt != *patIt)
							++strIt;
					}
				}
				else
				{
					if (*patIt != *strIt)
					{
						if (lastWildCardIt != tmpPattern.end())
						{
							// The last wildcard can match this incorrect sequence
							// rewind pattern to wildcard and keep searching
							patIt = lastWildCardIt;
							lastWildCardIt = tmpPattern.end();
						}
						else
						{
							// no wildwards left
							return false;
						}
					}
					else
					{
						++patIt;
						++strIt;
					}
				}

			}

			// If we reached the end of both the pattern and the string, we succeeded
			if (patIt == tmpPattern.end() && strIt == tmpStr.end())
				return true;
			else
				return false;
		}

		template <class T>
		static const typename BasicString<T> replaceAllInternal(const typename BasicString<T>& source, 
			const typename BasicString<T>& replaceWhat, const typename BasicString<T>& replaceWithWhat)
		{
			BasicString<T> result = source;
			BasicString<T>::size_type pos = 0;
			while(1)
			{
				pos = result.find(replaceWhat,pos);
				if (pos == BasicString<T>::npos) break;
				result.replace(pos,replaceWhat.size(), replaceWithWhat);
				pos += replaceWithWhat.size();
			}
			return result;
		}
    };

	/**
	 * @brief	Converts a narrow string to a wide string.
	 */
	BS_UTILITY_EXPORT WString toWString(const String& source);

	/**
	 * @brief	Converts a float to a string.
	 */
    BS_UTILITY_EXPORT WString toWString(float val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts a double to a string.
	 */
	BS_UTILITY_EXPORT WString toWString(double val, unsigned short precision = 6, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts a Radian to a string.
	 */
    BS_UTILITY_EXPORT WString toWString(Radian val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts a Degree to a string.
	 */
    BS_UTILITY_EXPORT WString toWString(Degree val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts an int to a string.
	 */
    BS_UTILITY_EXPORT WString toWString(int val, unsigned short width = 0, 
        char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts an unsigned int to a string.
	 */
    BS_UTILITY_EXPORT WString toWString(unsigned int val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts an 64bit integer to a string.
	 */
	BS_UTILITY_EXPORT WString toWString(INT64 val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts an 64bit unsigned to a string.
	 */
	BS_UTILITY_EXPORT WString toWString(UINT64 val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	* @brief	Converts an narrow char unsigned to a string.
	*/
	BS_UTILITY_EXPORT WString toWString(char val,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0));

	/**
	* @brief	Converts an wide bit char unsigned to a string.
	*/
	BS_UTILITY_EXPORT WString toWString(wchar_t val,
		unsigned short width = 0, char fill = ' ',
		std::ios::fmtflags flags = std::ios::fmtflags(0));

    /**
     * @brief	Converts a boolean to a string.
     *
     * @param	val  	Value to convert.
     * @param	yesNo	(optional) If set to true, result is "yes" or "no" instead of "true" or "false".
     */
    BS_UTILITY_EXPORT WString toWString(bool val, bool yesNo = false);

    /**
     * @brief	Converts a 2 dimensional vector to a string.
     * 			
	 * @note	Format is "x y".
     */
    BS_UTILITY_EXPORT WString toWString(const Vector2& val);

    /**
     * @brief	Converts a 3 dimensional vector to a string.
     * 			
	 * @note	Format is "x y z".
     */
    BS_UTILITY_EXPORT WString toWString(const Vector3& val);

    /**
     * @brief	Converts a 4 dimensional vector to a string.
     * 			
	 * @note	Format is "x y z w".
     */
    BS_UTILITY_EXPORT WString toWString(const Vector4& val);

    /**
     * @brief	Converts a 3x3 matrix to a string.
     * 			
	 * @note	Format is "00 01 02 10 11 12 20 21 22".
     */
    BS_UTILITY_EXPORT WString toWString(const Matrix3& val);

    /**
     * @brief	Converts a 4x4 matrix to a string.
     * 			
	 * @note	Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33".
     */
    BS_UTILITY_EXPORT WString toWString(const Matrix4& val);

    /**
     * @brief	Converts a Quaternion to a string.
     * 			
	 * @note	Format is "w x y z".
     */
    BS_UTILITY_EXPORT WString toWString(const Quaternion& val);

    /**
     * @brief	Converts a color to a string.
     * 			
	 * @note	Format is "r g b a".
     */
    BS_UTILITY_EXPORT WString toWString(const Color& val);

    /**
     * @brief	Converts a vector of strings into a single string where the substrings are
	 *			delimited by spaces.
     */
    BS_UTILITY_EXPORT WString toWString(const Vector<BansheeEngine::WString>& val);

	/**
	* @brief	Converts a wide string to a narrow string.
	*/
	BS_UTILITY_EXPORT String toString(const WString& source);

	/**
	 * @brief	Converts a float to a string.
	 */
    BS_UTILITY_EXPORT String toString(float val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts a double to a string.
	 */
	BS_UTILITY_EXPORT String toString(double val, unsigned short precision = 6, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts a Radian to a string.
	 */
    BS_UTILITY_EXPORT String toString(Radian val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts a Degree to a string.
	 */
    BS_UTILITY_EXPORT String toString(Degree val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts an int to a string.
	 */
    BS_UTILITY_EXPORT String toString(int val, unsigned short width = 0, 
        char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts an unsigned int to a string.
	 */
    BS_UTILITY_EXPORT String toString(unsigned int val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts a 64bit int to a string.
	 */
	BS_UTILITY_EXPORT String toString(INT64 val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/**
	 * @brief	Converts an 64bit unsigned int to a string.
	 */
	BS_UTILITY_EXPORT String toString(UINT64 val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /**
     * @brief	Converts a boolean to a string.
     *
     * @param	val  	true to value.
     * @param	yesNo	(optional) If set to true, result is "yes" or "no" instead of "true" or "false".
     */
    BS_UTILITY_EXPORT String toString(bool val, bool yesNo = false);

    /**
     * @brief	Converts a 2 dimensional vector to a string.
     * 			
	 * @note	Format is "x y".
     */
    BS_UTILITY_EXPORT String toString(const Vector2& val);

    /**
     * @brief	Converts a 3 dimensional vector to a string.
     * 			
	 * @note	Format is "x y z".
     */
    BS_UTILITY_EXPORT String toString(const Vector3& val);

    /**
     * @brief	Converts a 4 dimensional vector to a string.
     * 			
	 * @note	Format is "x y z w".
     */
    BS_UTILITY_EXPORT String toString(const Vector4& val);

    /**
     * @brief	Converts a 3x3 matrix to a string.
     * 			
	 * @note	Format is "00 01 02 10 11 12 20 21 22".
     */
    BS_UTILITY_EXPORT String toString(const Matrix3& val);

    /**
     * @brief	Converts a 4x4 matrix to a string.
     * 			
	 * @note	Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33".
     */
    BS_UTILITY_EXPORT String toString(const Matrix4& val);

    /**
     * @brief	Converts a Quaternion to a string.
     * 			
	 * @note	Format is "w x y z".
     */
    BS_UTILITY_EXPORT String toString(const Quaternion& val);

    /**
     * @brief	Converts a color to a string.
     * 			
	 * @note	Format is "r g b a".
     */
    BS_UTILITY_EXPORT String toString(const Color& val);

    /**
     * @brief	Converts a vector of strings into a single string where the substrings are
	 *			delimited by spaces.
     */
    BS_UTILITY_EXPORT String toString(const Vector<BansheeEngine::String>& val);

    /**
     * @brief	Converts a String to a float.
     *
     * @note	0.0f if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT float parseFloat(const String& val, float defaultValue = 0);

    /**
     * @brief	Converts a String to a whole number.
     *
     * @note	0 if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT int parseInt(const String& val, int defaultValue = 0);

    /**
     * @brief	Converts a String to a whole number.
     *
     * @note	0 if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT unsigned int parseUnsignedInt(const String& val, unsigned int defaultValue = 0);

    /**
     * @brief	Converts a String to a whole number.
     *
     * @note	0 if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT long parseLong(const String& val, long defaultValue = 0);

    /**
     * @brief	Converts a String to a whole number.
     *
     * @note	0 if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT unsigned long parseUnsignedLong(const String& val, unsigned long defaultValue = 0);

    /**
     * @brief	Converts a String to a boolean.
     *
	 * @note	Returns true if case-insensitive match of the start of the string
	 *			matches "true", "yes" or "1", false otherwise.
     */
    BS_UTILITY_EXPORT bool parseBool(const String& val, bool defaultValue = 0);

    /**
     * @brief	Checks the String is a valid number value.
     */
    BS_UTILITY_EXPORT bool isNumber(const String& val);

    /**
     * @brief	Converts a WString to a float.
     *
     * @note	0.0f if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT float parseFloat(const WString& val, float defaultValue = 0);

    /**
     * @brief	Converts a WString to a whole number.
     *
     * @note	0 if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT int parseInt(const WString& val, int defaultValue = 0);

    /**
     * @brief	Converts a WString to a whole number.
     *
     * @note	0 if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT unsigned int parseUnsignedInt(const WString& val, unsigned int defaultValue = 0);

    /**
     * @brief	Converts a WString to a whole number.
     *
     * @note	0 if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT long parseLong(const WString& val, long defaultValue = 0);

    /**
     * @brief	Converts a WString to a whole number.
     *
     * @note	0 if the value could not be parsed, otherwise the numeric version of the string.
     */
    BS_UTILITY_EXPORT unsigned long parseUnsignedLong(const WString& val, unsigned long defaultValue = 0);

    /**
     * @brief	Converts a WString to a boolean.
     *
	 * @note	Returns true if case-insensitive match of the start of the string
	 *			matches "true", "yes" or "1", false otherwise.
     */
    BS_UTILITY_EXPORT bool parseBool(const WString& val, bool defaultValue = 0);

    /**
     * @brief	Checks the WString is a valid number value.
     */
    BS_UTILITY_EXPORT bool isNumber(const WString& val);

	/**
	 * @brief	Helper method that throws an exception regarding a data overflow.
	 */
	void BS_UTILITY_EXPORT __string_throwDataOverflowException();

	/**
	 * @brief	RTTIPlainType specialization for String that allows strings be serialized as
	 * 			value types.
	 * 			
	 * @see		RTTIPlainType
	 */
	template<> struct RTTIPlainType<String>
	{	
		enum { id = 20 }; enum { hasDynamicSize = 1 };

		static void toMemory(const String& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);
			size -= sizeof(UINT32);
			memcpy(memory, data.data(), size); 
		}

		static UINT32 fromMemory(String& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			UINT32 stringSize = size - sizeof(UINT32);
			char* buffer = (char*)bs_alloc<ScratchAlloc>(stringSize + 1);
			memcpy(buffer, memory, stringSize); 
			buffer[stringSize] = '\0';
			data = String(buffer);

			bs_free<ScratchAlloc>(buffer);

			return size;
		}

		static UINT32 getDynamicSize(const String& data)	
		{ 
			UINT64 dataSize = data.size() * sizeof(String::value_type) + sizeof(UINT32);

#if BS_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				__string_throwDataOverflowException();
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	/**
	 * @brief	RTTIPlainType specialization for WString that allows strings be serialized as
	 * 			value types.
	 * 			
	 * @see		RTTIPlainType
	 */
	template<> struct RTTIPlainType<WString>
	{	
		enum { id = TID_WString }; enum { hasDynamicSize = 1 };

		static void toMemory(const WString& data, char* memory)
		{ 
			UINT32 size = getDynamicSize(data);

			memcpy(memory, &size, sizeof(UINT32));
			memory += sizeof(UINT32);
			size -= sizeof(UINT32);
			memcpy(memory, data.data(), size); 
		}

		static UINT32 fromMemory(WString& data, char* memory)
		{ 
			UINT32 size;
			memcpy(&size, memory, sizeof(UINT32)); 
			memory += sizeof(UINT32);

			UINT32 stringSize = size - sizeof(UINT32);
			WString::value_type* buffer = (WString::value_type*)bs_alloc<ScratchAlloc>(stringSize + sizeof(WString::value_type));
			memcpy(buffer, memory, stringSize); 

			UINT32 numChars =  stringSize / sizeof(WString::value_type);
			buffer[numChars] = L'\0';

			data = WString(buffer);

			bs_free<ScratchAlloc>(buffer);

			return size;
		}

		static UINT32 getDynamicSize(const WString& data)	
		{ 
			UINT64 dataSize = data.size() * sizeof(WString::value_type) + sizeof(UINT32);

#if BS_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				__string_throwDataOverflowException();
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 
}

/**
 * @brief	Hash value generator for SString.
 */
template<> 
struct std::hash<BansheeEngine::String>
{
	size_t operator()(const BansheeEngine::String& string) const
	{
		size_t hash = 0;
		for(size_t i = 0; i < string.size(); i++) 
			hash = 65599 * hash + string[i];
		return hash ^ (hash >> 16);
	}
};

/**
 * @brief	Hash value generator for WString.
 */
template<> 
struct std::hash<BansheeEngine::WString>
{
	size_t operator()(const BansheeEngine::WString& string) const
	{
		size_t hash = 0;
		for(size_t i = 0; i < string.size(); i++) 
			hash = 65599 * hash + string[i];
		return hash ^ (hash >> 16);
	}
};


#include "BsHString.h"