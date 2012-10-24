#include "CmMaterial.h"
#include "CmException.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmRenderSystemManager.h"
#include "CmRenderSystem.h"
#include "CmGpuProgramParams.h"
#include "CmGpuProgram.h"

namespace CamelotEngine
{
	void Material::setShader(ShaderPtr shader)
	{
		mShader = shader;

		mBestTechnique = nullptr;
		mParameters.clear();

		if(shader)
		{
			mBestTechnique = shader->getBestTechnique();

			if(mBestTechnique)
			{
				for(UINT32 i = 0; i < mBestTechnique->getNumPasses(); i++)
				{
					PassPtr curPass = mBestTechnique->getPass(i);

					ParamsPerPass params;

					GpuProgramPtr vertProgram = curPass->getVertexProgram();
					if(vertProgram)
						params.mVertParams = vertProgram->createParameters();

					GpuProgramPtr fragProgram = curPass->getFragmentProgram();
					if(fragProgram)
						params.mFragParams = fragProgram->createParameters();

					GpuProgramPtr geomProgram = curPass->getGeometryProgram();
					if(geomProgram)
						params.mGeomParams = geomProgram->createParameters();	

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

	void Material::setTexture(const String& name, TexturePtr value)
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

	void Material::applyPass(UINT32 passIdx)
	{
		throwIfNotInitialized();

		// TODO - I need to take care to minimize shader and shader constant (especially texture) changes
		RenderSystem* renderSystem = RenderSystemManager::getActive();

		PassPtr curPass = mBestTechnique->getPass(passIdx);
		ParamsPerPass params = mParameters[passIdx];

		GpuProgramPtr vertProgram = curPass->getVertexProgram();
		if(vertProgram)
		{
			renderSystem->bindGpuProgram(vertProgram->_getBindingDelegate());
			renderSystem->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, params.mVertParams, GPV_ALL);
		}

		GpuProgramPtr fragProgram = curPass->getFragmentProgram();
		if(fragProgram)
		{
			renderSystem->bindGpuProgram(fragProgram->_getBindingDelegate());
			renderSystem->bindGpuProgramParameters(GPT_FRAGMENT_PROGRAM, params.mFragParams, GPV_ALL);
		}

		GpuProgramPtr geomProgram = curPass->getGeometryProgram();
		if(geomProgram)
		{
			renderSystem->bindGpuProgram(geomProgram->_getBindingDelegate());
			renderSystem->bindGpuProgramParameters(GPT_GEOMETRY_PROGRAM, params.mGeomParams, GPV_ALL);
		}	
	}
}