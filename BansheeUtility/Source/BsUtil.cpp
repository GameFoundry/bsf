#include "BsUtil.h"
#include "ThirdParty/md5.h"

namespace BansheeEngine
{
	String md5(const WString& source)
	{
		MD5 md5;
		md5.update((UINT8*)source.c_str(), (UINT32)source.length());
		md5.finalize();

		UINT8* digest = (UINT8*)bs_alloc(16);
		md5.decdigest(digest, 16);

		String output((char*)digest);
		bs_free(digest);

		return output;
	}

	String md5(const String& source)
	{
		MD5 md5;
		md5.update((UINT8*)source.c_str(), (UINT32)source.length());
		md5.finalize();

		UINT8* digest = (UINT8*)bs_alloc(16);
		md5.decdigest(digest, 16);

		String output((char*)digest);
		bs_free(digest);

		return output;
	}
}