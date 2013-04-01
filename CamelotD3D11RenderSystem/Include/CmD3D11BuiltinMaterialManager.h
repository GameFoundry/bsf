#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmBuiltinMaterialManager.h"

namespace CamelotEngine
{
	/**
	 * @copydoc BuiltinMaterialManager
	 */
	class D3D11BuiltinMaterialManager : public BuiltinMaterialManager
	{
	public:
		MaterialHandle createSpriteTextMaterial() const;
		MaterialHandle createSpriteImageMaterial() const;

	protected:
		ShaderPtr mSpriteTextShader;
		ShaderPtr mSpriteImageShader;

		void onStartUp();

		void initSpriteTextShader();
		void initSpriteImageShader();
	};
}