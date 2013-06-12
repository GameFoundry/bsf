#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotFramework
{
	class CM_EXPORT FontManager : public Module<FontManager>
	{
	public:
		FontPtr create(Vector<FontData>::type& fontData) const;
		FontPtr createEmpty() const;
	};
}