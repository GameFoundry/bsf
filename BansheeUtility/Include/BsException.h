#pragma once

#include "BsPrerequisitesUtil.h"

#if defined(_MSC_VER)
#undef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace BansheeEngine
{
	/**
	 * @brief	Base class for all Banshee exceptions.
	 */
	class BS_UTILITY_EXPORT Exception : public std::exception
    {
    public:
		Exception(const char* type, const String& description, const String& source);
		Exception(const char* type, const String& description, const String& source, const char* file, long line);

        Exception(const Exception& rhs);
		~Exception() throw() {}

        void operator = (const Exception& rhs);

		/**
		 * @brief	Returns a string with the full description of the exception.
		 *
		 * @note	The description contains the error number, the description
		 *			supplied by the thrower, what routine threw the exception,
		 *			and will also supply extra platform-specific information
		 *			where applicable.
		 */
		virtual const String& getFullDescription() const;

		/**
		 * @brief	Gets the source function that threw the exception.
		 */
		virtual const String& getSource() const { return mSource; }

		/**
		 * @brief	Gets the source file name in which the exception was thrown.
		 */
		virtual const String& getFile() const { return mFile; }

        /**
         * @brief	Gets line number on which the exception was thrown.
         */
        virtual long getLine() const { return mLine; }

		/**
		 * @brief	Gets a short description about the exception.
		 */
		virtual const String& getDescription(void) const { return mDescription; }

		/**
		 * @brief	Overriden std::exception::what. Returns the same value as "getFullDescription".
		 */
		const char* what() const throw() { return getFullDescription().c_str(); }

	protected:
		long mLine;
		String mTypeName;
		String mDescription;
		String mSource;
		String mFile;
		mutable String mFullDesc;
    };

	/**
	 * @brief	Exception for signaling not implemented parts of the code.
	 */
	class BS_UTILITY_EXPORT NotImplementedException : public Exception 
	{
	public:
		NotImplementedException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("NotImplementedException", inDescription, inSource, inFile, inLine) {}
	};

	/**
	 * @brief	Exception for signaling file system errors when file could not be found.
	 */
	class BS_UTILITY_EXPORT FileNotFoundException : public Exception
	{
	public:
		FileNotFoundException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("FileNotFoundException", inDescription, inSource, inFile, inLine) {}
	};

	/**
	 * @brief	Exception for signaling general IO errors.
	 * 			
	 * @note	An example being failed to open a file or a network connection.
	 */
	class BS_UTILITY_EXPORT IOException : public Exception
	{
	public:
		IOException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("IOException", inDescription, inSource, inFile, inLine) {}
	};

	/**
	 * @brief	Exception for signaling not currently executing code in not in a valid state.
	 */
	class BS_UTILITY_EXPORT InvalidStateException : public Exception
	{
	public:
		InvalidStateException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("InvalidStateException", inDescription, inSource, inFile, inLine) {}
	};

	/**
	 * @brief	Exception for signaling not some parameters you have provided are not valid.
	 */
	class BS_UTILITY_EXPORT InvalidParametersException : public Exception
	{
	public:
		InvalidParametersException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("InvalidParametersException", inDescription, inSource, inFile, inLine) {}
	};

	/**
	 * @brief	Exception for signaling an internal error, normally something that shouldn't have happened or
	 * 			wasn't anticipated by the programmers of that system.
	 */
	class BS_UTILITY_EXPORT InternalErrorException : public Exception
	{
	public:
		InternalErrorException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("InternalErrorException", inDescription, inSource, inFile, inLine) {}
	};

	/**
	 * @brief	Exception for signaling an error in a rendering API.
	 */
	class BS_UTILITY_EXPORT RenderingAPIException : public Exception
	{
	public:
		RenderingAPIException(const String& inDescription, const String& inSource, const char* inFile, long inLine)
			: Exception("RenderingAPIException", inDescription, inSource, inFile, inLine) {}
	};

	/**
	 * @brief Macro for throwing exceptions that will automatically fill out function name, file name and line number of the exception.
	 */
#ifndef BS_EXCEPT
#define BS_EXCEPT(type, desc)	\
	{                           \
	static_assert((std::is_base_of<BansheeEngine::Exception, type##>::value), "Invalid exception type (" #type ") for BS_EXCEPT macro. It needs to derive from BansheeEngine::Exception."); \
	throw type##(desc, __PRETTY_FUNCTION__, __FILE__, __LINE__); \
	}
#endif
}

