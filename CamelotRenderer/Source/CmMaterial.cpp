#include "CmMaterial.h"
#include "CmException.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmRenderSystem.h"
#include "CmGpuProgramParams.h"
#include "CmGpuProgram.h"
#include "CmMaterialRTTI.h"

namespace CamelotEngine
{
	Material::Material()
	{
		// Material doesn't do anything render thread specific, so we can just initialize right away
		initialize_internal();
	}

	void Material::initialize_internal()
	{
		Resource::initialize_internal();
	}

	void Material::setShader(ShaderPtr shader)
	{
		mShader = shader;

		initBestTechnique();
	}

	void Material::initBestTechnique()
	{
		mBestTechnique = nullptr;
		mParameters.clear();

		if(mShader)
		{
			mBestTechnique = mShader->getBestTechnique();

			if(mBestTechnique)
			{
				for(UINT32 i = 0; i < mBestTechnique->getNumPasses(); i++)
				{
					PassPtr curPass = mBestTechnique->getPass(i);

					PassParametersPtr params = PassParametersPtr(new PassParameters());

					GpuProgramHandle vertProgram = curPass->getVertexProgram();
					if(vertProgram)
					{
						vertProgram.waitUntilLoaded();
						params->mVertParams = vertProgram->createParameters();
					}

					GpuProgramHandle fragProgram = curPass->getFragmentProgram();
					if(fragProgram)
					{
						fragProgram.waitUntilLoaded();
						params->mFragParams = fragProgram->createParameters();
					}

					GpuProgramHandle geomProgram = curPass->getGeometryProgram();
					if(geomProgram)
					{
						geomProgram.waitUntilLoaded();
						params->mGeomParams = geomProgram->createParameters();	
					}

					mParameters.push_back(params);
				}
			}
		}
	}

	void Material::throwIfNotInitialized() const
	{
		if(mShader == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Material does not have shader set.");
		}

		if(mBestTechnique == nullptr)
		{
			CM_EXCEPT(InternalErrorException, "Shader does not contain a supported technique.");
		}
	}

	void Material::setTexture(const String& name, TextureHandle& value)
	{
		throwIfNotInitialized();

		for(auto iter = mParameters.begin(); iter != mParameters.end(); ++iter)
		{
			PassParametersPtr params = *iter;

			if(params->mVertParams)
			{
				if(params->mVertParams->hasTexture(name))
					params->mVertParams->setTexture(name, value);
			}

			if(params->mFragParams)
			{
				if(params->mFragParams->hasTexture(name))
					params->mFragParams->setTexture(name, value);
			}

			if(params->mGeomParams)
			{
				if(params->mGeomParams->hasTexture(name))
					params->mGeomParams->setTexture(name, value);
			}
		}
	}

	void Material::setSamplerState(const String& name, SamplerStatePtr samplerState)
	{
		throwIfNotInitialized();

		for(auto iter = mParameters.begin(); iter != mParameters.end(); ++iter)
		{
			PassParametersPtr params = *iter;

			if(params->mVertParams)
			{
				if(params->mVertParams->hasSamplerState(name))
					params->mVertParams->setSamplerState(name, samplerState);
			}

			if(params->mFragParams)
			{
				if(params->mFragParams->hasSamplerState(name))
					params->mFragParams->setSamplerState(name, samplerState);
			}

			if(params->mGeomParams)
			{
				if(params->mGeomParams->hasSamplerState(name))
					params->mGeomParams->setSamplerState(name, samplerState);
			}
		}
	}

	void Material::setFloat(const String& name, float value)
	{
		throwIfNotInitialized();
		setParam(name, value);
	}

	void Material::setColor(const String& name, const Color& value)
	{
		throwIfNotInitialized();
		setParam(name, value);
	}

	void Material::setVec2(const String& name, const Vector2& value)
	{
		throwIfNotInitialized();
		setParam(name, value);
	}

	void Material::setVec3(const String& name, const Vector3& value)
	{
		throwIfNotInitialized();
		setParam(name, value);
	}

	void Material::setVec4(const String& name, const Vector4& value)
	{
		throwIfNotInitialized();
		setParam(name, value);
	}

	void Material::setMat3(const String& name, const Matrix3& value)
	{
		throwIfNotInitialized();
		setParam(name, value);
	}

	void Material::setMat4(const String& name, const Matrix4& value)
	{
		throwIfNotInitialized();
		setParam(name, value);
	}

	UINT32 Material::getNumPasses() const
	{
		throwIfNotInitialized();

		return mShader->getBestTechnique()->getNumPasses();
	}

	PassPtr Material::getPass(UINT32 passIdx) const
	{
		if(passIdx < 0 || passIdx >= mShader->getBestTechnique()->getNumPasses())
			CM_EXCEPT(InvalidParametersException, "Invalid pass index.");

		return mShader->getBestTechnique()->getPass(passIdx);
	}

	PassParametersPtr Material::getPassParameters(UINT32 passIdx) const
	{
		if(passIdx < 0 || passIdx >= mParameters.size())
			CM_EXCEPT(InvalidParametersException, "Invalid pass index.");

		return mParameters[passIdx];
	}

	RTTITypeBase* Material::getRTTIStatic()
	{
		return MaterialRTTI::instance();
	}

	RTTITypeBase* Material::getRTTI() const
	{
		return Material::getRTTIStatic();
	}
}