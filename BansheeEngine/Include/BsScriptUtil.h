#pragma once

#include "BsPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_EXPORT ScriptUtil
	{
	public:
		static CM::WString monoToWString(MonoString* str)
		{
			int len = mono_string_length(str);
			mono_unichar2* monoChars = mono_string_chars(str);

			CM::WString ret(len, '0');
			for(int i = 0; i < len; i++)
				ret[i] = monoChars[i];

			return ret;
		}
	};
}