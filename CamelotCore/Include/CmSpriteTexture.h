#pragma once

#include "CmPrerequisites.h"
#include "CmVector2.h"

namespace CamelotEngine
{
	class CM_EXPORT SpriteTexture
	{
	public:
		const HTexture& getBaseTexture() const { return mBaseTexture; }

	private:
		HTexture mBaseTexture;
		Vector2 mUVOffset;
		Vector2 mUVScale;
	};
}