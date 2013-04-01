#pragma once

#include "CmGLPrerequisites.h"
#include "CmBuiltinMaterialManager.h"

namespace CamelotEngine
{
	/**
	 * @copydoc BuiltinMaterialManager
	 */
	class GLBuiltinMaterialManager : public BuiltinMaterialManager
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