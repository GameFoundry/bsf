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

namespace CamelotFramework 
{
	const String StringUtil::BLANK;
	const WString StringUtil::WBLANK;

    void StringUtil::trim(String& str, bool left, bool right)
    {
        static const String delims = " \t\r";
        if(right)
            str.erase(str.find_last_not_of(delims)+1); // trim right
        if(left)
            str.erase(0, str.find_first_not_of(delims)); // trim left
    }

	void StringUtil::trim(WString& str, bool left, bool right)
	{
		static const WString delims = L" \t\r";
		if(right)
			str.erase(str.find_last_not_of(delims)+1); // trim right
		if(left)
			str.erase(0, str.find_first_not_of(delims)); // trim left
	}

    vector<String>::type StringUtil::split(const String& str, const String& delims, unsigned int maxSplits)
    {
        return splitInternal<char>(str, delims, maxSplits);
    }

	vector<WString>::type StringUtil::split(const WString& str, const WString& delims, unsigned int maxSplits)
	{
		return splitInternal<wchar_t>(str, delims, maxSplits);
	}

	vector<String>::type StringUtil::tokenise(const String& str, const String& singleDelims, const String& doubleDelims, unsigned int maxSplits)
	{
        return tokeniseInternal<char>(str, singleDelims, doubleDelims, maxSplits);
    }

	vector<WString>::type StringUtil::tokenise(const WString& str, const WString& singleDelims, const WString& doubleDelims, unsigned int maxSplits)
	{
		return tokeniseInternal<wchar_t>(str, singleDelims, doubleDelims, maxSplits);
	}

    void StringUtil::toLowerCase(String& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
    }

	void StringUtil::toLowerCase(WString& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), tolower);
	}

    void StringUtil::toUpperCase(String& str) 
    {
        std::transform(str.begin(), str.end(), str.begin(), toupper);
    }

	void StringUtil::toUpperCase(WString& str) 
	{
		std::transform(str.begin(), str.end(), str.begin(), toupper);
	}

    bool StringUtil::startsWith(const String& str, const String& pattern, bool lowerCase)
    {
        return startsWithInternal<char>(str, pattern, lowerCase);
    }

	bool StringUtil::startsWith(const WString& str, const WString& pattern, bool lowerCase)
	{
		return startsWithInternal<wchar_t>(str, pattern, lowerCase);
	}

    bool StringUtil::endsWith(const String& str, const String& pattern, bool lowerCase)
    {
        return endsWithInternal<char>(str, pattern, lowerCase);
    }

	bool StringUtil::endsWith(const WString& str, const WString& pattern, bool lowerCase)
	{
		return endsWithInternal<wchar_t>(str, pattern, lowerCase);
	}

    bool StringUtil::match(const String& str, const String& pattern, bool caseSensitive)
    {
        return matchInternal<char>(str, pattern, caseSensitive);
    }

	bool StringUtil::match(const WString& str, const WString& pattern, bool caseSensitive)
	{
		return matchInternal<wchar_t>(str, pattern, caseSensitive);
	}

	const String StringUtil::replaceAll(const String& source, const String& replaceWhat, const String& replaceWithWhat)
	{
		return replaceAllInternal<char>(source, replaceWhat, replaceWithWhat);
	}

	const WString StringUtil::replaceAll(const WString& source, const WString& replaceWhat, const WString& replaceWithWhat)
	{
		return replaceAllInternal<wchar_t>(source, replaceWhat, replaceWithWhat);
	}

	/************************************************************************/
	/* 						VARIOUS TO STRING CONVERSIONS                   */
	/************************************************************************/

	WString toWString(const String& source)
	{
		return WString(source.begin(), source.end());
	}

	String toString(const WString& source)
	{
		return String(source.begin(), source.end());
	}

	String toString(float val, unsigned short precision, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.precision(precision);
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	String toString(Radian val, unsigned short precision, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		return toString(val.valueAngleUnits(), precision, width, fill, flags);
	}

	String toString(Degree val, unsigned short precision, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		return toString(val.valueAngleUnits(), precision, width, fill, flags);
	}

	String toString(int val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

#if CM_ARCH_TYPE == CM_ARCHITECTURE_64 || CM_PLATFORM == CM_PLATFORM_APPLE
	String toString(unsigned int val, unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	String toString(size_t val, unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
#if CM_COMPILER == CM_COMPILER_MSVC

	String toString(unsigned long val, unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

#endif

#else
	String toString(size_t val, unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	String toString(unsigned long val, unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	String toString(unsigned long long int val, unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}
#endif
	String toString(long val, 
		unsigned short width, char fill, std::ios::fmtflags flags)
	{
		StringStream stream;
		stream.width(width);
		stream.fill(fill);
		if (flags)
			stream.setf(flags);
		stream << val;
		return stream.str();
	}

	String toString(const Vector2& val)
	{
		StringStream stream;
		stream << val.x << " " << val.y;
		return stream.str();
	}

	String toString(const Vector3& val)
	{
		StringStream stream;
		stream << val.x << " " << val.y << " " << val.z;
		return stream.str();
	}

	String toString(const Vector4& val)
	{
		StringStream stream;
		stream << val.x << " " << val.y << " " << val.z << " " << val.w;
		return stream.str();
	}

	String toString(const Matrix3& val)
	{
		StringStream stream;
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

	String toString(const Matrix4& val)
	{
		StringStream stream;
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

	String toString(const Quaternion& val)
	{
		StringStream stream;
		stream  << val.w << " " << val.x << " " << val.y << " " << val.z;
		return stream.str();
	}

	String toString(const Color& val)
	{
		StringStream stream;
		stream << val.r << " " << val.g << " " << val.b << " " << val.a;
		return stream.str();
	}

	String toString(const vector<CamelotFramework::String>::type& val)
	{
		StringStream stream;
		vector<CamelotFramework::String>::type::const_iterator i, iend, ibegin;
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

	float parseFloat(const String& val, float defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		float ret = defaultValue;
		str >> ret;

		return ret;
	}

	int parseInt(const String& val, int defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		int ret = defaultValue;
		str >> ret;

		return ret;
	}

	unsigned int parseUnsignedInt(const String& val, unsigned int defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		unsigned int ret = defaultValue;
		str >> ret;

		return ret;
	}

	long parseLong(const String& val, long defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		long ret = defaultValue;
		str >> ret;

		return ret;
	}

	unsigned long parseUnsignedLong(const String& val, unsigned long defaultValue)
	{
		// Use istringstream for direct correspondence with toString
		StringStream str(val);
		unsigned long ret = defaultValue;
		str >> ret;

		return ret;
	}

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

	bool isNumber(const String& val)
	{
		StringStream str(val);
		float tst;
		str >> tst;
		return !str.fail() && str.eof();
	}

	void __string_throwDataOverflowException()
	{
		CM_EXCEPT(InternalErrorException, "Data overflow! Size doesn't fit into 32 bits.");
	}
}
