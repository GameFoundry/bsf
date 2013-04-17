#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmBuiltinMaterialManager.h"

namespace CamelotFramework
{
	/**
	 * @copydoc BuiltinMaterialManager
	 */
	class D3D11BuiltinMaterialManager : public BuiltinMaterialManager
	{
	public:
		HMaterial createSpriteTextMaterial() const;
		HMaterial createSpriteImageMaterial() const;

	protected:
		ShaderPtr mSpriteTextShader;
		ShaderPtr mSpriteImageShader;

		void onStartUp();

		void initSpriteTextShader();
		void initSpriteImageShader();
	};
}