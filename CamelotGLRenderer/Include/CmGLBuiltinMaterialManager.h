#pragma once

#include "CmGLPrerequisites.h"
#include "CmBuiltinMaterialManager.h"

namespace CamelotFramework
{
	/**
	 * @copydoc BuiltinMaterialManager
	 */
	class GLBuiltinMaterialManager : public BuiltinMaterialManager
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