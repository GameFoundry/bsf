#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	class CM_EXPORT FontManager : public Module<FontManager>
	{
	public:
		FontPtr create(vector<FontData>::type& fontData) const;
		FontPtr createEmpty() const;
	};
}