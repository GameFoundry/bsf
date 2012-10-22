#include "CmMaterial.h"
#include "CmException.h"
#include "CmShader.h"
#include "CmTechnique.h"

namespace CamelotEngine
{
	void Material::setShader(ShaderPtr shader)
	{
		mShader = shader;
	}

	void Material::throwIfNotInitialized() const
	{
		if(mShader == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Material does not have shader set.");
		}
	}

	void Material::setTexture(const String& name, TexturePtr value)
	{
		throwIfNotInitialized();
	}

	void Material::setFloat(const String& name, float value)
	{
		throwIfNotInitialized();
	}

	void Material::setColor(const String& name, const Color& value)
	{
		throwIfNotInitialized();
	}

	void Material::setVec(const String& name, const Vector2& value)
	{
		throwIfNotInitialized();
	}

	void Material::setVec(const String& name, const Vector3& value)
	{
		throwIfNotInitialized();
	}

	void Material::setVec(const String& name, const Vector4& value)
	{
		throwIfNotInitialized();
	}

	void Material::setMat(const String& name, const Matrix3& value)
	{
		throwIfNotInitialized();
	}

	void Material::setMat(const String& name, const Matrix4& value)
	{
		throwIfNotInitialized();
	}

	UINT32 Material::getNumPasses() const
	{
		throwIfNotInitialized();

		return mShader->getBestTechnique()->getNumPasses();
	}

	void Material::applyPass(UINT32 passIdx)
	{
		throwIfNotInitialized();


	}
}