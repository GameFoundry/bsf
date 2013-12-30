#pragma once

#include "BsMonoPrerequisites.h"
#include "CmException.h"
#include "CmDebug.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT MonoUtil
	{
	public:
		static CM::WString monoToWString(MonoString* str)
		{
			if(str == nullptr)
				return CM::StringUtil::WBLANK;

			int len = mono_string_length(str);
			mono_unichar2* monoChars = mono_string_chars(str);

			CM::WString ret(len, '0');
			for(int i = 0; i < len; i++)
				ret[i] = monoChars[i];

			return ret;
		}

		static MonoString* wstringToMono(MonoDomain* domain, const CM::WString& str)
		{
			CM::UINT32 len = (CM::UINT32)str.length();
			mono_unichar2* monoChars = (mono_unichar2*)CM::cm_alloc<mono_unichar2>(len);

			for(CM::UINT32 i = 0; i < len; i++)
				monoChars[i] = str[i];

			MonoString* monoString = mono_string_new_utf16(domain, monoChars, len);

			CM::cm_free(monoChars);
			return monoString;
		}

		static void throwIfException(MonoObject* exception)
		{
			if(exception != nullptr)
			{
				::MonoClass* exceptionClass = mono_object_get_class(exception);
				::MonoProperty* exceptionMsgProp = mono_class_get_property_from_name(exceptionClass, "Message");
				::MonoMethod* exceptionMsgGetter = mono_property_get_get_method(exceptionMsgProp);
				MonoString* exceptionMsg = (MonoString*)mono_runtime_invoke(exceptionMsgGetter, exception, nullptr, nullptr);

				::MonoProperty* exceptionStackProp = mono_class_get_property_from_name(exceptionClass, "StackTrace");
				::MonoMethod* exceptionStackGetter = mono_property_get_get_method(exceptionStackProp);
				MonoString* exceptionStackTrace = (MonoString*)mono_runtime_invoke(exceptionStackGetter, exception, nullptr, nullptr);

				CM::String msg =  "Managed exception: " + toString(monoToWString(exceptionMsg)) + "\n" + toString(monoToWString(exceptionStackTrace));

				LOGERR(msg);
				CM_EXCEPT(CM::InternalErrorException, msg);
			}
		}
	};
}