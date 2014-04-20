#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace BansheeEngine
{
	class CM_EXPORT FontManager : public Module<FontManager>
	{
	public:
		FontPtr create(const Vector<FontData>::type& fontData) const;
		FontPtr createEmpty() const;
	};
}