#include "CmMaterial.h"
#include "CmException.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmRenderSystemManager.h"
#include "CmRenderSystem.h"
#include "CmGpuProgramParams.h"
#include "CmGpuProgram.h"
#include "CmMaterialRTTI.h"

namespace CamelotEngine
{
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

					GpuProgramRef vertProgram = curPass->getVertexProgram();
					if(vertProgram)
						params->mVertParams = vertProgram->createParameters();

					GpuProgramRef fragProgram = curPass->getFragmentProgram();
					if(fragProgram)
						params->mFragParams = fragProgram->createParameters();

					GpuProgramRef geomProgram = curPass->getGeometryProgram();
					if(geomProgram)
						params->mGeomParams = geomProgram->createParameters();	

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

	void Material::setTexture(const String& name, TextureRef& value)
	{
		throwIfNotInitialized();
		setParam(name, value);
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

	void Material::applyPassParameters(UINT32 passIdx)
	{
		throwIfNotInitialized();

		// TODO - I need to take care to minimize shader and shader constant (especially texture) changes
		RenderSystem* renderSystem = RenderSystemManager::getActive();

		PassPtr curPass = mBestTechnique->getPass(passIdx);
		PassParametersPtr params = mParameters[passIdx];

		GpuProgramRef vertProgram = curPass->getVertexProgram();
		if(vertProgram)
			renderSystem->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, params->mVertParams, GPV_ALL);

		GpuProgramRef fragProgram = curPass->getFragmentProgram();
		if(fragProgram)
			renderSystem->bindGpuProgramParameters(GPT_FRAGMENT_PROGRAM, params->mFragParams, GPV_ALL);

		GpuProgramRef geomProgram = curPass->getGeometryProgram();
		if(geomProgram)
			renderSystem->bindGpuProgramParameters(GPT_GEOMETRY_PROGRAM, params->mGeomParams, GPV_ALL);
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