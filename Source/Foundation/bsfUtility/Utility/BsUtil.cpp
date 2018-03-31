//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Prerequisites/BsPrerequisitesUtil.h"
#include "ThirdParty/md5.h"

namespace bs
{
	String md5(const WString& source)
	{
		MD5 md5;
		md5.update((UINT8*)source.data(), (UINT32)source.length() * sizeof(WString::value_type));
		md5.finalize();

		UINT8 digest[16];
		md5.decdigest(digest, sizeof(digest));

		String buf;
		buf.reserve(33);
		for (int i = 0; i < 16; i++)
		{
			//C++ 17 will allow for direct use of .data()
			snprintf(&(buf[0]) + i * 2, 3, "%02x", digest[i]);
		}
		buf[32] = L'\0';

		return buf;
	}

	String md5(const String& source)
	{
		MD5 md5;
		md5.update((UINT8*)source.data(), (UINT32)source.length() * sizeof(String::value_type));
		md5.finalize();

		UINT8 digest[16];
		md5.decdigest(digest, sizeof(digest));

		String buf;
		buf.reserve(33);
		for (int i = 0; i < 16; i++)
		{
			//C++ 17 will allow for direct use of .data()
			snprintf(&(buf[0]) + i * 2, 3, "%02x", digest[i]);
		}
		buf[32] = '\0';

		return buf;
	}
}
