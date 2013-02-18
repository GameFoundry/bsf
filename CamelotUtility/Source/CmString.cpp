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

#include "CmString.h"
#include "CmColor.h"
#include "CmMath.h"
#include "CmMatrix3.h"
#include "CmMatrix4.h"
#include "CmQuaternion.h"
#include "CmVector2.h"
#include "CmVector3.h"
#include "CmVector4.h"
#include "CmException.h"

namespace CamelotEngine {

	//-----------------------------------------------------------------------
	const String StringUtil::BLANK;
	//-----------------------------------------------------------------------
    void StringUtil::trim(String& str, bool left, bool right)
    {
        /*
        size_t lspaces, rspaces, len = length(), i;

        lspaces = rspaces = 0;

        if( left )
        {
            // Find spaces / tabs on the left
            for( i = 0;
                i < len && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r');
                ++lspaces, ++i );
        }
        
        if( right && lspaces < len )
        {
            // Find spaces / tabs on the right
            for( i = len - 1;
                i >= 0 && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r');
                rspaces++, i-- );
        }

        *this = substr(lspaces, len-lspaces-rspaces);
        */
        static const String delims = " \t\r";
        if(right)
            str.erase(str.find_last_not_of(delims)+1); // trim right
        if(left)
            str.erase(0, str.find_first_not_of(delims)); // trim left
    }

    //-----------------------------------------------------------------------
    std::vector<CamelotEngine::String> StringUtil::split( const String& str, const String& delims, unsigned int maxSplits)
    {
        std::vector<CamelotEngine::String> ret;
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
            else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
            {
                // Copy the rest of the string
                ret.push_back( str.substr(start) );
                break;
            }
            else
            {
                // Copy up to delimiter
                ret.push_back( str.substr(start, pos - start) );
                start = pos + 1;
            }
            // parse up to next real data
            start = str.find_first_not_of(delims, start);
            ++numSplits;

        } while (pos != String::npos);



        return ret;
    }
	//-----------------------------------------------------------------------
	std::vector<CamelotEngine::String> StringUtil::tokenise( const String& str, const String& singleDelims, const String& doubleDelims, unsigned int maxSplits)
	{
        std::vector<CamelotEngine::String> ret;
        // Pre-allocate some space for performance
        ret.reserve(maxSplits ? maxSplits+1 : 10);    // 10 is guessed capacity for most case

        unsigned int numSplits = 0;
		String delims = singleDelims + doubleDelims;

		// Use STL methods 
        size_t start, pos;
		char curDoubleDelim = 0;
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
				char curDelim = str.at(pos);
				if (doubleDelims.find_first_of(curDelim) != String::npos)
				{
					curDoubleDelim = curDelim;
				}
                // Do nothing
                start = pos + 1;
            }
            else if (pos == String::npos || (maxSplits && numSplits == maxSplits))
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

        } while (pos != String::npos);

        return ret;
    }
    //-----------------------------------------------------------------------
    void StringUtil::toLowerCase(String& str)
    {
        std::transform(
            str.begin(),
            str.end(),
            str.begin(),
			tolower);
    }

    //-----------------------------------------------------------------------
    void StringUtil::toUpperCase(String& str) 
    {
        std::transform(
            str.begin(),
            str.end(),
            str.begin(),
			toupper);
    }
    //-----------------------------------------------------------------------
    bool StringUtil::startsWith(const String& str, const String& pattern, bool lowerCase)
    {
        size_t thisLen = str.length();
        size_t patternLen = pattern.length();
        if (thisLen < patternLen || patternLen == 0)
            return false;

        String startOfThis = str.substr(0, patternLen);
        if (lowerCase)
            StringUtil::toLowerCase(startOfThis);

        return (startOfThis == pattern);
    }
    //-----------------------------------------------------------------------
    bool StringUtil::endsWith(const String& str, const String& pattern, bool lowerCase)
    {
        size_t thisLen = str.length();
        size_t patternLen = pattern.length();
        if (thisLen < patternLen || patternLen == 0)
            return false;

        String endOfThis = str.substr(thisLen - patternLen, patternLen);
        if (lowerCase)
            StringUtil::toLowerCase(endOfThis);

        return (endOfThis == pattern);
    }
    //-----------------------------------------------------------------------
    String StringUtil::standardisePath(const String& init)
    {
        String path = init;

        std::replace( path.begin(), path.end(), '\\', '/' );
        if( path[path.length() - 1] != '/' )
            path += '/';

        return path;
    }
    //-----------------------------------------------------------------------
    void StringUtil::splitFilename(const String& qualifiedName, 
        String& outBasename, String& outPath)
    {
        String path = qualifiedName;
        // Replace \ with / first
        std::replace( path.begin(), path.end(), '\\', '/' );
        // split based on final /
        size_t i = path.find_last_of('/');

        if (i == String::npos)
        {
            outPath.clear();
			outBasename = qualifiedName;
        }
        else
        {
            outBasename = path.substr(i+1, path.size() - i - 1);
            outPath = path.substr(0, i+1);
        }

    }
	//-----------------------------------------------------------------------
	void StringUtil::splitBaseFilename(const CamelotEngine::String& fullName, 
		CamelotEngine::String& outBasename, CamelotEngine::String& outExtention)
	{
		size_t i = fullName.find_last_of(".");
		if (i == CamelotEngine::String::npos)
		{
			outExtention.clear();
			outBasename = fullName;
		}
		else
		{
			outExtention = fullName.substr(i+1);
			outBasename = fullName.substr(0, i);
		}
	}
	// ----------------------------------------------------------------------------------------------------------------------------------------------
	void StringUtil::splitFullFilename(	const CamelotEngine::String& qualifiedName, 
		CamelotEngine::String& outBasename, CamelotEngine::String& outExtention, CamelotEngine::String& outPath )
	{
		CamelotEngine::String fullName;
		splitFilename( qualifiedName, fullName, outPath );
		splitBaseFilename( fullName, outBasename, outExtention );
	}
    //-----------------------------------------------------------------------
    bool StringUtil::match(const String& str, const String& pattern, bool caseSensitive)
    {
        String tmpStr = str;
		String tmpPattern = pattern;
        if (!caseSensitive)
        {
            StringUtil::toLowerCase(tmpStr);
            StringUtil::toLowerCase(tmpPattern);
        }

        String::const_iterator strIt = tmpStr.begin();
        String::const_iterator patIt = tmpPattern.begin();
		String::const_iterator lastWildCardIt = tmpPattern.end();
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
		{
        	return true;
		}
		else
		{
			return false;
		}

    }
	//-----------------------------------------------------------------------
	const String StringUtil::replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat)
	{
		String result = source;
        String::size_type pos = 0;
		while(1)
		{
			pos = result.find(replaceWhat,pos);
			if (pos == String::npos) break;
			result.replace(pos,replaceWhat.size(),replaceWithWhat);
            pos += replaceWithWhat.size();
		}
		return result;
	}
	//-----------------------------------------------------------------------
	String toString(float val, unsigned short precision, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.precision(precision);
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(Radian val, unsigned short precision, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		return toString(val.valueAngleUnits(), precision, width, fill, flags);
	}
	//-----------------------------------------------------------------------
	String toString(Degree val, unsigned short precision, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		return toString(val.valueAngleUnits(), precision, width, fill, flags);
	}
	//-----------------------------------------------------------------------
	String toString(int val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
#if CM_ARCH_TYPE == CM_ARCHITECTURE_64 || CM_PLATFORM == CM_PLATFORM_APPLE
	String toString(unsigned int val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(size_t val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
#if CM_COMPILER == CM_COMPILER_MSVC
	//-----------------------------------------------------------------------
	String toString(unsigned long val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

#endif
	//-----------------------------------------------------------------------
#else
	String toString(size_t val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(unsigned long val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(unsigned long long int val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
#endif
	String toString(long val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		stringstream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(const Vector2& val)
	{
		stringstream stream;
		stream << val.x << " " << val.y;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(const Vector3& val)
	{
		stringstream stream;
		stream << val.x << " " << val.y << " " << val.z;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(const Vector4& val)
	{
		stringstream stream;
		stream << val.x << " " << val.y << " " << val.z << " " << val.w;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(const Matrix3& val)
	{
		stringstream stream;
		stream << val[0][0] << " " 
			<< val[0][1] << " "             
			<< val[0][2] << " "             
			<< val[1][0] << " "             
			<< val[1][1] << " "             
			<< val[1][2] << " "             
			<< val[2][0] << " "             
			<< val[2][1] << " "             
			<< val[2][2];
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(bool val, bool yesNo)
	{
		if (val)
		{
			if (yesNo)
			{
				return "yes";
			}
			else
			{
				return "true";
			}
		}
		else
			if (yesNo)
			{
				return "no";
			}
			else
			{
				return "false";
			}
	}
	//-----------------------------------------------------------------------
	String toString(const Matrix4& val)
	{
		stringstream stream;
		stream << val[0][0] << " " 
			<< val[0][1] << " "             
			<< val[0][2] << " "             
			<< val[0][3] << " "             
			<< val[1][0] << " "             
			<< val[1][1] << " "             
			<< val[1][2] << " "             
			<< val[1][3] << " "             
			<< val[2][0] << " "             
			<< val[2][1] << " "             
			<< val[2][2] << " "             
			<< val[2][3] << " "             
			<< val[3][0] << " "             
			<< val[3][1] << " "             
			<< val[3][2] << " "             
			<< val[3][3];
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(const Quaternion& val)
	{
		stringstream stream;
		stream  << val.w << " " << val.x << " " << val.y << " " << val.z;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(const Color& val)
	{
		stringstream stream;
		stream << val.r << " " << val.g << " " << val.b << " " << val.a;
		return stream.str();
	}
	//-----------------------------------------------------------------------
	String toString(const std::vector<CamelotEngine::String>& val)
	{
		stringstream stream;
		std::vector<CamelotEngine::String>::const_iterator i, iend, ibegin;
		ibegin = val.begin();
		iend = val.end();
		for (i = ibegin; i != iend; ++i)
		{
			if (i != ibegin)
				stream << " ";

			stream << *i; 
		}
		return stream.str();
	}
	//-----------------------------------------------------------------------
	float parseFloat(const String& val, float defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		float ret = defaultValue;
		str >> ret;

		return ret;
	}
	//-----------------------------------------------------------------------
	int parseInt(const String& val, int defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		int ret = defaultValue;
		str >> ret;

		return ret;
	}
	//-----------------------------------------------------------------------
	unsigned int parseUnsignedInt(const String& val, unsigned int defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		unsigned int ret = defaultValue;
		str >> ret;

		return ret;
	}
	//-----------------------------------------------------------------------
	long parseLong(const String& val, long defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		long ret = defaultValue;
		str >> ret;

		return ret;
	}
	//-----------------------------------------------------------------------
	unsigned long parseUnsignedLong(const String& val, unsigned long defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		unsigned long ret = defaultValue;
		str >> ret;

		return ret;
	}
	//-----------------------------------------------------------------------
	bool parseBool(const String& val, bool defaultValue)
	{
		if ((StringUtil::startsWith(val, "true") || StringUtil::startsWith(val, "yes")
			|| StringUtil::startsWith(val, "1")))
			return true;
		else if ((StringUtil::startsWith(val, "false") || StringUtil::startsWith(val, "no")
			|| StringUtil::startsWith(val, "0")))
			return false;
		else
			return defaultValue;
	}
	//-----------------------------------------------------------------------
	std::vector<CamelotEngine::String> parseStringVector(const String& val)
	{
		return StringUtil::split(val);
	}
	//-----------------------------------------------------------------------
	bool isNumber(const String& val)
	{
		StringStream str(val);
		float tst;
		str >> tst;
		return !str.fail() && str.eof();
	}
	//----------------------------------------------------------------------
	void __string_throwDataOverflowException()
	{
		CM_EXCEPT(InternalErrorException, "Data overflow! Size doesn't fit into 32 bits.");
	}
}
