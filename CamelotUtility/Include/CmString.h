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
#ifndef _String_H__
#define _String_H__

#include "CmPrerequisitesUtil.h"

// If we're using the GCC 3.1 C++ Std lib
#if CM_COMPILER == CM_COMPILER_GNUC && CM_COMP_VER >= 310 && !defined(STLPORT)

// For gcc 4.3 see http://gcc.gnu.org/gcc-4.3/changes.html
#   if CM_COMP_VER >= 430
#       include <tr1/unordered_map> 
#   else
#       include <ext/hash_map>
namespace __gnu_cxx
{
    template <> struct hash< CamelotFramework::_StringBase >
    {
        size_t operator()( const CamelotFramework::_StringBase _stringBase ) const
        {
            /* This is the PRO-STL way, but it seems to cause problems with VC7.1
               and in some other cases (although I can't recreate it)
            hash<const char*> H;
            return H(_stringBase.c_str());
            */
            /** This is our custom way */
            register size_t ret = 0;
            for( CamelotFramework::_StringBase::const_iterator it = _stringBase.begin(); it != _stringBase.end(); ++it )
                ret = 5 * ret + *it;

            return ret;
        }
    };
}
#   endif

#endif

namespace CamelotFramework {

#if CM_WCHAR_T_STRINGS
		typedef std::wstring _StringBase;
#else
		typedef std::string _StringBase;
#endif

#if CM_WCHAR_T_STRINGS
		typedef std::basic_stringstream<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t> > _StringStreamBase;
#else
		typedef std::basic_stringstream<char,std::char_traits<char>,std::allocator<char> > _StringStreamBase;
#endif

		typedef _StringBase String;
		typedef _StringStreamBase StringStream;
		typedef StringStream stringstream;

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
		typedef StringStream StrStreamType;

        /** Removes any whitespace characters, be it standard space or
            TABs and so on.
            @remarks
                The user may specify wether they want to trim only the
                beginning or the end of the String ( the default action is
                to trim both).
        */
        static void trim( String& str, bool left = true, bool right = true );

        /** Returns a StringVector that contains all the substrings delimited
            by the characters in the passed <code>delims</code> argument.
            @param
                delims A list of delimiter characters to split by
            @param
                maxSplits The maximum number of splits to perform (0 for unlimited splits). If this
                parameters is > 0, the splitting process will stop after this many splits, left to right.
        */
		static vector<String>::type split( const String& str, const String& delims = "\t\n ", unsigned int maxSplits = 0);

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
		static vector<String>::type tokenise( const String& str, const String& delims = "\t\n ", const String& doubleDelims = "\"", unsigned int maxSplits = 0);

		/** Lower-cases all the characters in the string.
        */
        static void toLowerCase( String& str );

        /** Upper-cases all the characters in the string.
        */
        static void toUpperCase( String& str );


        /** Returns whether the string begins with the pattern passed in.
        @param pattern The pattern to compare with.
        @param lowerCase If true, the start of the string will be lower cased before
            comparison, pattern should also be in lower case.
        */
        static bool startsWith(const String& str, const String& pattern, bool lowerCase = true);

        /** Returns whether the string ends with the pattern passed in.
        @param pattern The pattern to compare with.
        @param lowerCase If true, the end of the string will be lower cased before
            comparison, pattern should also be in lower case.
        */
        static bool endsWith(const String& str, const String& pattern, bool lowerCase = true);

        /** Method for standardising paths - use forward slashes only, end with slash.
        */
        static String standardisePath( const String &init);

        /** Method for splitting a fully qualified filename into the base name
            and path.
        @remarks
            Path is standardised as in standardisePath
        */
        static void splitFilename(const String& qualifiedName,
            String& outBasename, String& outPath);

		/** Method for splitting a fully qualified filename into the base name,
		extension and path.
		@remarks
		Path is standardised as in standardisePath
		*/
		static void splitFullFilename(const CamelotFramework::String& qualifiedName, 
			CamelotFramework::String& outBasename, CamelotFramework::String& outExtention, 
			CamelotFramework::String& outPath);

		/** Method for splitting a filename into the base name
		and extension.
		*/
		static void splitBaseFilename(const CamelotFramework::String& fullName, 
			CamelotFramework::String& outBasename, CamelotFramework::String& outExtention);


        /** Simple pattern-matching routine allowing a wildcard pattern.
        @param str String to test
        @param pattern Pattern to match against; can include simple '*' wildcards
        @param caseSensitive Whether the match is case sensitive or not
        */
        static bool match(const String& str, const String& pattern, bool caseSensitive = true);


		/** replace all instances of a sub-string with a another sub-string.
		@param source Source string
		@param replaceWhat Sub-string to find and replace
		@param replaceWithWhat Sub-string to replace with (the new sub-string)
		@returns An updated string with the sub-string replaced
		*/
		static const String replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat);

        /// Constant blank string, useful for returning by ref where local does not exist
        static const String BLANK;
    };


#if CM_COMPILER == CM_COMPILER_GNUC && CM_COMP_VER >= 310 && !defined(STLPORT)
#   if CM_COMP_VER < 430
	typedef ::__gnu_cxx::hash< _StringBase > _StringHash;
#   else
	typedef ::std::tr1::hash< _StringBase > _StringHash;
#   endif
#elif CM_COMPILER == CM_COMPILER_MSVC && CM_COMP_VER >= 1600 && !defined(STLPORT) // VC++ 10.0
	typedef ::std::tr1::hash< _StringBase > _StringHash;
#elif !defined( _STLP_HASH_FUN_H )
	typedef stdext::hash_compare< _StringBase, std::less< _StringBase > > _StringHash;
#else
	typedef std::hash< _StringBase > _StringHash;
#endif 
	/** @} */
	/** @} */

	    /** Converts a float to a String. */
    CM_UTILITY_EXPORT String toString(float val, unsigned short precision = 6, 
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
#if CM_ARCH_TYPE == CM_ARCHITECTURE_64
    /** Converts an unsigned int to a String. */
    CM_UTILITY_EXPORT String toString(unsigned int val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );
    /** Converts a size_t to a String. */
    CM_UTILITY_EXPORT String toString(size_t val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );
    #if CM_COMPILER == CM_COMPILER_MSVC
            /** Converts an unsigned long to a String. */
            CM_UTILITY_EXPORT String toString(unsigned long val, 
                unsigned short width = 0, char fill = ' ', 
                std::ios::fmtflags flags = std::ios::fmtflags(0) );

    #endif
#else
    /** Converts a size_t to a String. */
    CM_UTILITY_EXPORT String toString(size_t val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );
    /** Converts an unsigned long to a String. */
    CM_UTILITY_EXPORT String toString(unsigned long val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );
	/** Converts a long long to a String. */
	CM_UTILITY_EXPORT String toString(unsigned long long int val, 
		unsigned short width = 0, char fill = ' ', 
		std::ios::fmtflags flags = std::ios::fmtflags(0) );
#endif
    /** Converts a long to a String. */
    CM_UTILITY_EXPORT String toString(long val, 
        unsigned short width = 0, char fill = ' ', 
        std::ios::fmtflags flags = std::ios::fmtflags(0) );

	/** Global conversion methods **/

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
    CM_UTILITY_EXPORT String toString(const std::vector<CamelotFramework::String>& val);

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

    /** Pareses a StringVector from a string.
    @remarks
        Strings must not contain spaces since space is used as a delimiter in
        the output.
    */
    CM_UTILITY_EXPORT std::vector<CamelotFramework::String> parseStringVector(const String& val);
    /** Checks the String is a valid number value. */
    CM_UTILITY_EXPORT bool isNumber(const String& val);
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
			char* buffer = (char*)CM_NEW_BYTES(stringSize + 1, ScratchAlloc);
			memcpy(buffer, memory, stringSize); 
			buffer[stringSize] = '\0';
			data = String(buffer);

			CM_DELETE_BYTES(buffer, ScratchAlloc);

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

} // namespace CamelotFramework

#endif // _String_H__
