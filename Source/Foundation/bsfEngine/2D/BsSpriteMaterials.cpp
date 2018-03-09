//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "2D/BsSpriteMaterials.h"
#include "Resources/BsBuiltinResources.h"
#include "RenderAPI/BsGpuBuffer.h"
#include "Material/BsMaterial.h"

namespace bs
{
	SpriteImageTransparentMaterial::SpriteImageTransparentMaterial()
		:SpriteMaterial(0, BuiltinResources::instance().createSpriteImageMaterial())
	{ }

	SpriteImageOpaqueMaterial::SpriteImageOpaqueMaterial()
		: SpriteMaterial(1, BuiltinResources::instance().createSpriteNonAlphaImageMaterial())
	{ }

	SpriteTextMaterial::SpriteTextMaterial()
		: SpriteMaterial(2, BuiltinResources::instance().createSpriteTextMaterial())
	{ }

	SpriteLineMaterial::SpriteLineMaterial()
		: SpriteMaterial(3, BuiltinResources::instance().createSpriteLineMaterial())
	{ }
}