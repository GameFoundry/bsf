#include "BsTestOutput.h"
#include "BsException.h"

namespace BansheeEngine
{
	void ExceptionTestOutput::outputFail(const String& desc, const String& function, const String& file, long line)
	{
		throw UnitTestException(desc, function, file.c_str(), line);
	}
}