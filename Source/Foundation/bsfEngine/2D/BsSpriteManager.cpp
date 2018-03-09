//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "2D/BsSpriteManager.h"
#include "2D/BsSpriteMaterials.h"

namespace bs
{
	SpriteManager::SpriteManager()
	{
		SpriteMaterial* imageTransparentMat = registerMaterial<SpriteImageTransparentMaterial>();
		SpriteMaterial* imageOpaqueMat = registerMaterial<SpriteImageOpaqueMaterial>();
		SpriteMaterial* textMat = registerMaterial<SpriteTextMaterial>();
		SpriteMaterial* lineMat = registerMaterial<SpriteLineMaterial>();

		builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::ImageTransparent] = imageTransparentMat->getId();
		builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::ImageOpaque] = imageOpaqueMat->getId();
		builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::Text] = textMat->getId();
		builtinMaterialIds[(UINT32)BuiltinSpriteMaterialType::Line] = lineMat->getId();
	}

	SpriteManager::~SpriteManager()
	{
		for(auto& entry : mMaterials)
			bs_delete(entry.second);
	}

	SpriteMaterial* SpriteManager::getMaterial(UINT32 id) const
	{
		auto iterFind = mMaterials.find(id);
		if (iterFind != mMaterials.end())
			return iterFind->second;

		return nullptr;
	}
}