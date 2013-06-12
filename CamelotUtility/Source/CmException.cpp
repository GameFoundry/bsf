#include "CmException.h"

namespace CamelotFramework
{
	Exception::Exception(const char* type, const String& desc, const String& src) 
		:mLine(0), mTypeName(type), mDescription(desc), mSource(src)
    { }

    Exception::Exception(const char* type, const String& desc, const String& src, const char* file, long line)
		:mLine(line), mTypeName(type), mDescription(desc), mSource(src), mFile(file)
    { }

    Exception::Exception(const Exception& rhs)
        : mLine(rhs.mLine), mTypeName(rhs.mTypeName), mDescription(rhs.mDescription), 
		mSource(rhs.mSource), mFile(rhs.mFile)
    { }

    void Exception::operator = (const Exception& rhs)
    {
        mDescription = rhs.mDescription;
        mSource = rhs.mSource;
        mFile = rhs.mFile;
        mLine = rhs.mLine;
		mTypeName = rhs.mTypeName;
    }

    const String& Exception::getFullDescription(void) const
    {
		if (mFullDesc.empty())
		{
			StringStream desc;

			desc<< "CAMELOT EXCEPTION(" << mTypeName << "): "
				<< mDescription 
				<< " in " << mSource;

			if(mLine > 0)
			{
				desc << " at " << mFile << " (line " << mLine << ")";
			}

			mFullDesc = desc.str();
		}

		return mFullDesc;
    }
}