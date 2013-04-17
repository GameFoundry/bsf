#pragma once

#include "CmPrerequisitesUtil.h"

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#if defined(_MSC_VER)
#undef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace CamelotFramework
{
	class CM_UTILITY_EXPORT Exception : public std::exception
    {
    protected:
        long mLine;
		String mTypeName;
        String mDescription;
        String mSource;
        String mFile;
		mutable String mFullDesc;
    public:
        /** Default constructor.
        */
		Exception(const char* type, const String& description, const String& source);

        /** Advanced constructor.
        */
		Exception(const char* type, const String& description, const String& source, const char* file, long line);

        /** Copy constructor.
        */
        Exception(const Exception& rhs);

		/// Needed for  compatibility with std::exception
		~Exception() throw() {}

        /** Assignment operator.
        */
        void operator = (const Exception& rhs);

        /** Returns a string with the full description of this error.
            @remarks
                The description contains the error number, the description
                supplied by the thrower, what routine threw the exception,
                and will also supply extra platform-specific information
                where applicable. For example - in the case of a rendering
                library error, the description of the error will include both
                the place in which OGRE found the problem, and a text
                description from the 3D rendering library, if available.
        */
		virtual const String& getFullDescription() const;

        /** Gets the source function.
        */
		virtual const String& getSource() const { return mSource; }

        /** Gets source file name.
        */
		virtual const String& getFile() const { return mFile; }

        /** Gets line number.
        */
        virtual long getLine() const { return mLine; }

		/** Returns a string with only the 'description' field of this exception. Use 
			getFullDescriptionto get a full description of the error including line number,
			error number and what function threw the exception.
        */
		virtual const String& getDescription(void) const { return mDescription; }

		/// Override std::exception::what
		const char* what() const throw() { return getFullDescription().c_str(); }
    };

	class CM_UTILITY_EXPORT NotImplementedException : public Exception 
	{
	public:
		NotImplementedException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("NotImplementedException", inDescription, inSource, inFile, inLine) {}
	};
	class CM_UTILITY_EXPORT FileNotFoundException : public Exception
	{
	public:
		FileNotFoundException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("FileNotFoundException", inDescription, inSource, inFile, inLine) {}
	};
	class CM_UTILITY_EXPORT IOException : public Exception
	{
	public:
		IOException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("IOException", inDescription, inSource, inFile, inLine) {}
	};
	class CM_UTILITY_EXPORT InvalidStateException : public Exception
	{
	public:
		InvalidStateException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("InvalidStateException", inDescription, inSource, inFile, inLine) {}
	};
	class CM_UTILITY_EXPORT InvalidParametersException : public Exception
	{
	public:
		InvalidParametersException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("InvalidParametersException", inDescription, inSource, inFile, inLine) {}
	};
	class CM_UTILITY_EXPORT ItemIdentityException : public Exception
	{
	public:
		ItemIdentityException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("ItemIdentityException", inDescription, inSource, inFile, inLine) {}
	};
	class CM_UTILITY_EXPORT InternalErrorException : public Exception
	{
	public:
		InternalErrorException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("InternalErrorException", inDescription, inSource, inFile, inLine) {}
	};
	class CM_UTILITY_EXPORT RenderingAPIException : public Exception
	{
	public:
		RenderingAPIException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("RenderingAPIException", inDescription, inSource, inFile, inLine) {}
	};
	class CM_UTILITY_EXPORT RuntimeAssertionException : public Exception
	{
	public:
		RuntimeAssertionException(const std::string& inDescription, const std::string& inSource, const char* inFile, long inLine)
			: Exception("RuntimeAssertionException", inDescription, inSource, inFile, inLine) {}
	};

#ifndef CM_EXCEPT
#define CM_EXCEPT(type, desc)	\
	{                           \
	BOOST_STATIC_ASSERT_MSG((boost::is_base_of<CamelotFramework::Exception, type##>::value), "Invalid exception type (" #type ") for CM_EXCEPT macro. It needs to derive from CamelotFramework::Exception."); \
	throw type##(desc, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	}
#endif
}

