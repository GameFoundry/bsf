//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "2D/BsSpriteMaterials.h"
#include "Resources/BsBuiltinResources.h"
#include "RenderAPI/BsGpuBuffer.h"
#include "Material/BsMaterial.h"

namespace bs
{
	UINT32 getMaterialId(SpriteMaterialTransparency transparency, bool animated)
	{
		switch(transparency)
		{
		default:
		case SpriteMaterialTransparency::Opaque: return animated ? 3 : 0;
		case SpriteMaterialTransparency::Alpha: return animated ? 4 : 1;
		case SpriteMaterialTransparency::Premultiplied: return animated ? 5 : 2;
		}
	}

	ShaderVariation getMaterialVariation(SpriteMaterialTransparency transparency, bool animated)
	{
		return ShaderVariation(SmallVector<ShaderVariation::Param, 4>({
			ShaderVariation::Param("TRANSPARENCY", (INT32)transparency),
			ShaderVariation::Param("ANIMATED", animated)
			}));
	}

	SpriteImageMaterial::SpriteImageMaterial(SpriteMaterialTransparency transparency, bool animated)
		:SpriteMaterial(
			getMaterialId(transparency, animated),
			BuiltinResources::instance().createSpriteImageMaterial(),
			getMaterialVariation(transparency, animated),
			!animated)
	{ }

	SpriteTextMaterial::SpriteTextMaterial()
		: SpriteMaterial(6, BuiltinResources::instance().createSpriteTextMaterial())
	{ }

	SpriteLineMaterial::SpriteLineMaterial()
		: SpriteMaterial(7, BuiltinResources::instance().createSpriteLineMaterial())
	{ }
}
