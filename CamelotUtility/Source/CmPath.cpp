#include "CmPath.h"

namespace CamelotFramework
{
	const WPath PathUtil::BLANK = WPath();

	Path toPath(const String& p) { return p.c_str(); }
	WPath toPath(const WString& p) { return p.c_str(); }

	String toString(const Path& p) { return String(p.string().c_str()); }
	WString toWString(const WPath& p) { return WString(p.wstring().c_str()); }
}