/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework 
{
	template <typename T>
	struct BasicString 
	{ 
		typedef typename std::basic_string<T, std::char_traits<T>, StdAlloc<T>> type; 
	}; 

	template <typename T>
	struct BasicStringStream
	{ 
		typedef typename std::basic_stringstream<T, std::char_traits<T>, StdAlloc<T>> type; 
	}; 

	typedef BasicString<wchar_t>::type WString;
	typedef BasicString<char>::type String;

	typedef BasicStringStream<wchar_t>::type WStringStream;
	typedef BasicStringStream<char>::type StringStream;

	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup General
	*  @{
	*/

    /** Utility class for manipulating Strings.  */
    class CM_UTILITY_EXPORT StringUtil
    {
	public:
        /** Removes any whitespace characters, be it standard space or
            TABs and so on.
            @remarks
                The user may specify wether they want to trim only the
                beginning or the end of the String ( the default action is
                to trim both).
        */
        static void trim(String& str, bool left = true, bool right = true);

        /** Removes any whitespace characters, be it standard space or
            TABs and so on.
            @remarks
                The user may specify wether they want to trim only the
                beginning or the end of the String ( the default action is
                to trim both).
        */
        static void trim(WString& str, bool left = true, bool right = true);

        /** Removes specified characters.
            @remarks
                The user may specify wether they want to trim only the
                beginning or the end of the String ( the default action is
                to trim both).
        */
        static void trim(String& str, const String& delims, bool left = true, bool right = true);

        /** Removes specified characters.
            @remarks
                The user may specify wether they want to trim only the
                beginning or the end of the String ( the default action is
                to trim both).
        */
        static void trim(WString& str, const WString& delims, bool left = true, bool right = true);

        /** Returns a StringVector that contains all the substrings delimited
            by the characters in the passed <code>delims</code> argument.
            @param
                delims A list of delimiter characters to split by
            @param
                maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
                parameters is > 0, the splitting process will stop after this many splits, left to right.
        */
		static Vector<String>::type split(const String& str, const String& delims = "\t\n ", unsigned int maxSplits = 0);

		/** Returns a StringVector that contains all the substrings delimited
            by the characters in the passed <code>delims</code> argument.
            @param
                delims A list of delimiter characters to split by
            @param
                maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
                parameters is > 0, the splitting process will stop after this many splits, left to right.
        */
		static Vector<WString>::type split(const WString& str, const WString& delims = L"\t\n ", unsigned int maxSplits = 0);

		/** Returns a StringVector that contains all the substrings delimited
            by the characters in the passed <code>delims</code> argument, 
			or in the <code>doubleDelims</code> argument, which is used to include (normal) 
			delimeters in the tokenised string. For example, "strings like this".
            @param
                delims A list of delimiter characters to split by
			@param
                delims A list of double delimeters characters to tokenise by
            @param
                maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
                parameters is > 0, the splitting process will stop after this many splits, left to right.
        */
		static Vector<String>::type tokenise(const String& str, const String& delims = "\t\n ", const String& doubleDelims = "\"", unsigned int maxSplits = 0);

		/** Returns a StringVector that contains all the substrings delimited
            by the characters in the passed <code>delims</code> argument, 
			or in the <code>doubleDelims</code> argument, which is used to include (normal) 
			delimeters in the tokenised string. For example, "strings like this".
            @param
                delims A list of delimiter characters to split by
			@param
                delims A list of double delimeters characters to tokenise by
            @param
                maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
                parameters is > 0, the splitting process will stop after this many splits, left to right.
        */
		static Vector<WString>::type tokenise(const WString& str, const WString& delims = L"\t\n ", const WString& doubleDelims = L"\"", unsigned int maxSplits = 0);

		/** Lower-cases all the characters in the string.
        */
        static void toLowerCase(String& str);

		/** Lower-cases all the characters in the string.
        */
        static void toLowerCase(WString& str);

        /** Upper-cases all the characters in the string.
        */
        static void toUpperCase(String& str);

        /** Upper-cases all the characters in the string.
        */
        static void toUpperCase(WString& str);

        /** Returns whether the string begins with the pattern passed in.
        @param pattern The pattern to compare with.
        @param lowerCase If true, the start of the string will be lower cased before
            comparison, pattern should also be in lower case.
        */
        static bool startsWith(const String& str, const String& pattern, bool lowerCase = true);

        /** Returns whether the string begins with the pattern passed in.
        @param pattern The pattern to compare with.
        @param lowerCase If true, the start of the string will be lower cased before
            comparison, pattern should also be in lower case.
        */
        static bool startsWith(const WString& str, const WString& pattern, bool lowerCase = true);

        /** Returns whether the string ends with the pattern passed in.
        @param pattern The pattern to compare with.
        @param lowerCase If true, the end of the string will be lower cased before
            comparison, pattern should also be in lower case.
        */
        static bool endsWith(const String& str, const String& pattern, bool lowerCase = true);

        /** Returns whether the string ends with the pattern passed in.
        @param pattern The pattern to compare with.
        @param lowerCase If true, the end of the string will be lower cased before
            comparison, pattern should also be in lower case.
        */
        static bool endsWith(const WString& str, const WString& pattern, bool lowerCase = true);

        /** Simple pattern-matching routine allowing a wildcard pattern.
        @param str String to test
        @param pattern Pattern to match against; can include simple '*' wildcards
        @param caseSensitive Whether the match is case sensitive or not
        */
        static bool match(const String& str, const String& pattern, bool caseSensitive = true);

		/** Simple pattern-matching routine allowing a wildcard pattern.
        @param str String to test
        @param pattern Pattern to match against; can include simple '*' wildcards
        @param caseSensitive Whether the match is case sensitive or not
        */
        static bool match(const WString& str, const WString& pattern, bool caseSensitive = true);

		/** Replace all instances of a sub-string with a another sub-string.
		@param source Source string
		@param replaceWhat Sub-string to find and replace
		@param replaceWithWhat Sub-string to replace with (the new sub-string)
		@returns An updated string with the sub-string replaced
		*/
		static const String replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat);

		/** Replace all instances of a sub-string with a another sub-string.
		@param source Source string
		@param replaceWhat Sub-string to find and replace
		@param replaceWithWhat Sub-string to replace with (the new sub-string)
		@returns An updated string with the sub-string replaced
		*/
		static const WString replaceAll(const WString& source, const WString& replaceWhat, const WString& replaceWithWhat);

        /// Constant blank string, useful for returning by ref where local does not exist
        static const String BLANK;

		/// Constant blank string, useful for returning by ref where local does not exist
		static const WString WBLANK;

	private:
		template <class T>
		static typename Vector<typename BasicString<T>::type>::type splitInternal(const typename BasicString<T>::type& str, const typename BasicString<T>::type& delims, unsigned int maxSplits)
		{
			Vector<BasicString<T>::type>::type ret;
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
				else if (pos == BasicString<T>::type::npos || (maxSplits && numSplits == maxSplits))
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

			} while (pos != BasicString<T>::type::npos);

			return ret;
		}

		template <class T>
		static typename Vector<typename BasicString<T>::type>::type tokeniseInternal(const typename BasicString<T>::type& str, const typename BasicString<T>::type& singleDelims, 
			const typename BasicString<T>::type& doubleDelims, unsigned int maxSplits)
		{
			Vector<BasicString<T>::type>::type ret;
			// Pre-allocate some space for performance
			ret.reserve(maxSplits ? maxSplits + 1 : 10);    // 10 is guessed capacity for most case

			unsigned int numSplits = 0;
			BasicString<T>::type delims = singleDelims + doubleDelims;

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
					if (doubleDelims.find_first_of(curDelim) != BasicString<T>::type::npos)
					{
						curDoubleDelim = curDelim;
					}
					// Do nothing
					start = pos + 1;
				}
				else if (pos == BasicString<T>::type::npos || (maxSplits && numSplits == maxSplits))
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

			} while (pos != BasicString<T>::type::npos);

			return ret;
		}

		template <class T>
		static bool startsWithInternal(const typename BasicString<T>::type& str, const typename BasicString<T>::type& pattern, bool lowerCase)
		{
			size_t thisLen = str.length();
			size_t patternLen = pattern.length();
			if (thisLen < patternLen || patternLen == 0)
				return false;

			BasicString<T>::type startOfThis = str.substr(0, patternLen);
			if (lowerCase)
				StringUtil::toLowerCase(startOfThis);

			return (startOfThis == pattern);
		}

		template <class T>
		static bool endsWithInternal(const typename BasicString<T>::type& str, const typename BasicString<T>::type& pattern, bool lowerCase)
		{
			size_t thisLen = str.length();
			size_t patternLen = pattern.length();
			if (thisLen < patternLen || patternLen == 0)
				return false;

			BasicString<T>::type endOfThis = str.substr(thisLen - patternLen, patternLen);
			if (lowerCase)
				StringUtil::toLowerCase(endOfThis);

			return (endOfThis == pattern);
		}

		template <class T>
		static bool matchInternal(const typename BasicString<T>::type& str, const typename BasicString<T>::type& pattern, bool caseSensitive)
		{
			BasicString<T>::type tmpStr = str;
			BasicString<T>::type tmpPattern = pattern;
			if (!caseSensitive)
			{
				StringUtil::toLowerCase(tmpStr);
				StringUtil::toLowerCase(tmpPattern);
			}

			BasicString<T>::type::const_iterator strIt = tmpStr.begin();
			BasicString<T>::type::const_iterator patIt = tmpPattern.begin();
			BasicString<T>::type::const_iterator lastWildCardIt = tmpPattern.end();
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
		static const typename BasicString<T>::type replaceAllInternal(const typename BasicString<T>::type& source, 
			const typename BasicString<T>::type& replaceWhat, const typename BasicString<T>::type& replaceWithWhat)
		{
			BasicString<T>::type result = source;
			BasicString<T>::type::size_type pos = 0;
			while(1)
			{
				pos = result.find(replaceWhat,pos);
				if (pos == BasicString<T>::type::npos) break;
				result.replace(pos,replaceWhat.size(), replaceWithWhat);
				pos += replaceWithWhat.size();
			}
			return result;
		}
    };


	/**
	* @brief	Converts a narrow string to a wide string.
	*/
	CM_UTILITY_EXPORT WString toWString(const String& source);

	/** Converts a float to a WString. */
    CM_UTILITY_EXPORT WString toWString(float val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Converts a double to a WString. */
	CM_UTILITY_EXPORT WString toWString(double val, unsigned short precision = 6, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts a Radian to a WString. */
    CM_UTILITY_EXPORT WString toWString(Radian val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts a Degree to a WString. */
    CM_UTILITY_EXPORT WString toWString(Degree val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts an int to a WString. */
    CM_UTILITY_EXPORT WString toWString(int val, unsigned short width = 0, 
        char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts an unsigned int to a WString. */
    CM_UTILITY_EXPORT WString toWString(unsigned int val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Converts a long to a WString. */
	CM_UTILITY_EXPORT WString toWString(long val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts an unsigned long to a WString. */
    CM_UTILITY_EXPORT WString toWString(unsigned long val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Converts an INT64 to a WString. */
	CM_UTILITY_EXPORT WString toWString(INT64 val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Converts an UINT64 to a WString. */
	CM_UTILITY_EXPORT WString toWString(UINT64 val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts a boolean to a WString. 
    @param yesNo If set to true, result is 'yes' or 'no' instead of 'true' or 'false'
    */
    CM_UTILITY_EXPORT WString toWString(bool val, bool yesNo = false);

	/** Converts a Vector2 to a WString. 
    @remarks
        Format is "x y" (i.e. 2x float values, space delimited)
    */
    CM_UTILITY_EXPORT WString toWString(const Vector2& val);

    /** Converts a Vector3 to a WString. 
    @remarks
        Format is "x y z" (i.e. 3x float values, space delimited)
    */
    CM_UTILITY_EXPORT WString toWString(const Vector3& val);

	/** Converts a Vector4 to a WString. 
    @remarks
        Format is "x y z w" (i.e. 4x float values, space delimited)
    */
    CM_UTILITY_EXPORT WString toWString(const Vector4& val);

    /** Converts a Matrix3 to a WString. 
    @remarks
        Format is "00 01 02 10 11 12 20 21 22" where '01' means row 0 column 1 etc.
    */
    CM_UTILITY_EXPORT WString toWString(const Matrix3& val);

    /** Converts a Matrix4 to a WString. 
    @remarks
        Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33" where 
        '01' means row 0 column 1 etc.
    */
    CM_UTILITY_EXPORT WString toWString(const Matrix4& val);

    /** Converts a Quaternion to a WString. 
    @remarks
        Format is "w x y z" (i.e. 4x float values, space delimited)
    */
    CM_UTILITY_EXPORT WString toWString(const Quaternion& val);

    /** Converts a ColourValue to a WString. 
    @remarks
        Format is "r g b a" (i.e. 4x float values, space delimited). 
    */
    CM_UTILITY_EXPORT WString toWString(const Color& val);

    /** Converts a StringVector to a WString.
    @remarks
        Strings must not contain spaces since space is used as a delimiter in
        the output.
    */
    CM_UTILITY_EXPORT WString toWString(const Vector<CamelotFramework::WString>::type& val);

	/**
	* @brief	Converts a wide string to a narrow string.
	*/
	CM_UTILITY_EXPORT String toString(const WString& source);

	/** Converts a float to a String. */
    CM_UTILITY_EXPORT String toString(float val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Converts a double to a String. */
	CM_UTILITY_EXPORT String toString(double val, unsigned short precision = 6, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts a Radian to a String. */
    CM_UTILITY_EXPORT String toString(Radian val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts a Degree to a String. */
    CM_UTILITY_EXPORT String toString(Degree val, unsigned short precision = 6, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts an int to a String. */
    CM_UTILITY_EXPORT String toString(int val, unsigned short width = 0, 
        char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts an unsigned int to a String. */
    CM_UTILITY_EXPORT String toString(unsigned int val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Converts a long to a String. */
	CM_UTILITY_EXPORT String toString(long val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts an unsigned long to a String. */
    CM_UTILITY_EXPORT String toString(unsigned long val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Converts an unsigned long to a String. */
	CM_UTILITY_EXPORT String toString(INT64 val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Converts an unsigned long to a String. */
	CM_UTILITY_EXPORT String toString(UINT64 val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );

    /** Converts a boolean to a String. 
    @param yesNo If set to true, result is 'yes' or 'no' instead of 'true' or 'false'
    */
    CM_UTILITY_EXPORT String toString(bool val, bool yesNo = false);

	/** Converts a Vector2 to a String. 
    @remarks
        Format is "x y" (i.e. 2x float values, space delimited)
    */
    CM_UTILITY_EXPORT String toString(const Vector2& val);

    /** Converts a Vector3 to a String. 
    @remarks
        Format is "x y z" (i.e. 3x float values, space delimited)
    */
    CM_UTILITY_EXPORT String toString(const Vector3& val);

	/** Converts a Vector4 to a String. 
    @remarks
        Format is "x y z w" (i.e. 4x float values, space delimited)
    */
    CM_UTILITY_EXPORT String toString(const Vector4& val);

    /** Converts a Matrix3 to a String. 
    @remarks
        Format is "00 01 02 10 11 12 20 21 22" where '01' means row 0 column 1 etc.
    */
    CM_UTILITY_EXPORT String toString(const Matrix3& val);

    /** Converts a Matrix4 to a String. 
    @remarks
        Format is "00 01 02 03 10 11 12 13 20 21 22 23 30 31 32 33" where 
        '01' means row 0 column 1 etc.
    */
    CM_UTILITY_EXPORT String toString(const Matrix4& val);

    /** Converts a Quaternion to a String. 
    @remarks
        Format is "w x y z" (i.e. 4x float values, space delimited)
    */
    CM_UTILITY_EXPORT String toString(const Quaternion& val);

    /** Converts a ColourValue to a String. 
    @remarks
        Format is "r g b a" (i.e. 4x float values, space delimited). 
    */
    CM_UTILITY_EXPORT String toString(const Color& val);

    /** Converts a StringVector to a string.
    @remarks
        Strings must not contain spaces since space is used as a delimiter in
        the output.
    */
    CM_UTILITY_EXPORT String toString(const Vector<CamelotFramework::String>::type& val);

    /** Converts a String to a float. 
    @returns
        0.0 if the value could not be parsed, otherwise the float version of the String.
    */
    CM_UTILITY_EXPORT float parseFloat(const String& val, float defaultValue = 0);

    /** Converts a String to a whole number. 
    @returns
        0.0 if the value could not be parsed, otherwise the numeric version of the String.
    */
    CM_UTILITY_EXPORT int parseInt(const String& val, int defaultValue = 0);

    /** Converts a String to a whole number. 
    @returns
        0.0 if the value could not be parsed, otherwise the numeric version of the String.
    */
    CM_UTILITY_EXPORT unsigned int parseUnsignedInt(const String& val, unsigned int defaultValue = 0);

    /** Converts a String to a whole number. 
    @returns
        0.0 if the value could not be parsed, otherwise the numeric version of the String.
    */
    CM_UTILITY_EXPORT long parseLong(const String& val, long defaultValue = 0);

    /** Converts a String to a whole number. 
    @returns
        0.0 if the value could not be parsed, otherwise the numeric version of the String.
    */
    CM_UTILITY_EXPORT unsigned long parseUnsignedLong(const String& val, unsigned long defaultValue = 0);

    /** Converts a String to a boolean. 
    @remarks
        Returns true if case-insensitive match of the start of the string
		matches "true", "yes" or "1", false otherwise.
    */
    CM_UTILITY_EXPORT bool parseBool(const String& val, bool defaultValue = 0);

    /** Checks the String is a valid number value. */
    CM_UTILITY_EXPORT bool isNumber(const String& val);

/** Converts a String to a float. 
    @returns
        0.0 if the value could not be parsed, otherwise the float version of the String.
    */
    CM_UTILITY_EXPORT float parseFloat(const WString& val, float defaultValue = 0);

    /** Converts a String to a whole number. 
    @returns
        0.0 if the value could not be parsed, otherwise the numeric version of the String.
    */
    CM_UTILITY_EXPORT int parseInt(const WString& val, int defaultValue = 0);

    /** Converts a String to a whole number. 
    @returns
        0.0 if the value could not be parsed, otherwise the numeric version of the String.
    */
    CM_UTILITY_EXPORT unsigned int parseUnsignedInt(const WString& val, unsigned int defaultValue = 0);

    /** Converts a String to a whole number. 
    @returns
        0.0 if the value could not be parsed, otherwise the numeric version of the String.
    */
    CM_UTILITY_EXPORT long parseLong(const WString& val, long defaultValue = 0);

    /** Converts a String to a whole number. 
    @returns
        0.0 if the value could not be parsed, otherwise the numeric version of the String.
    */
    CM_UTILITY_EXPORT unsigned long parseUnsignedLong(const WString& val, unsigned long defaultValue = 0);

    /** Converts a String to a boolean. 
    @remarks
        Returns true if case-insensitive match of the start of the string
		matches "true", "yes" or "1", false otherwise.
    */
    CM_UTILITY_EXPORT bool parseBool(const WString& val, bool defaultValue = 0);

    /** Checks the String is a valid number value. */
    CM_UTILITY_EXPORT bool isNumber(const WString& val);

	/** @} */

	void CM_UTILITY_EXPORT __string_throwDataOverflowException();

	/**
	 * @brief	Strings need to copy their data in a slightly more intricate way than just memcpy.
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
			char* buffer = (char*)cm_alloc<ScratchAlloc>(stringSize + 1);
			memcpy(buffer, memory, stringSize); 
			buffer[stringSize] = '\0';
			data = String(buffer);

			cm_free<ScratchAlloc>(buffer);

			return size;
		}

		static UINT32 getDynamicSize(const String& data)	
		{ 
			UINT64 dataSize = data.size() * sizeof(String::value_type) + sizeof(UINT32);

#if CM_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				__string_throwDataOverflowException();
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

	/**
	 * @brief	Strings need to copy their data in a slightly more intricate way than just memcpy.
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
			WString::value_type* buffer = (WString::value_type*)cm_alloc<ScratchAlloc>(stringSize + 1);
			memcpy(buffer, memory, stringSize); 
			buffer[stringSize] = '\0';
			data = WString(buffer);

			cm_free<ScratchAlloc>(buffer);

			return size;
		}

		static UINT32 getDynamicSize(const WString& data)	
		{ 
			UINT64 dataSize = data.size() * sizeof(WString::value_type) + sizeof(UINT32);

#if CM_DEBUG_MODE
			if(dataSize > std::numeric_limits<UINT32>::max())
			{
				__string_throwDataOverflowException();
			}
#endif

			return (UINT32)dataSize;
		}	
	}; 

} // namespace CamelotFramework

template<> 
struct std::hash<CamelotFramework::String>
{
	size_t operator()(const CamelotFramework::String& string) const
	{
		size_t hash = 0;
		for(size_t i = 0; i < string.size(); i++) 
			hash = 65599 * hash + string[i];
		return hash ^ (hash >> 16);
	}
};

template<> 
struct std::hash<CamelotFramework::WString>
{
	size_t operator()(const CamelotFramework::WString& string) const
	{
		size_t hash = 0;
		for(size_t i = 0; i < string.size(); i++) 
			hash = 65599 * hash + string[i];
		return hash ^ (hash >> 16);
	}
};


#include "CmHString.h"