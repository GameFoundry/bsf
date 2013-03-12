#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	class CM_EXPORT FontManager : public Module<FontManager>
	{
	public:
		FontPtr create(const FONT_DESC& fontDesc, vector<TexturePtr>::type texturePages) const;
		FontPtr createEmpty() const;
	};
}