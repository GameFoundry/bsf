#include "BsLightRendering.h"
#include "BsMaterial.h"
#include "BsShader.h"
#include "BsRenderBeast.h"
#include "BsRenderTargets.h"
#include "BsGpuParams.h"
#include "BsLight.h"

namespace BansheeEngine
{
	void PerLightParams::setParameters(const LightCore* light)
	{
		// Note: I could just copy the data directly to the parameter buffer if I ensured the parameter
		// layout matches

		Vector4 positionAndType = (Vector4)light->getPosition();

		switch (light->getType())
		{
		case LightType::Directional:
			positionAndType.w = 0;
			break;
		case LightType::Point:
			positionAndType.w = 0.3f;
			break;
		case LightType::Spot:
			positionAndType.w = 0.8f;
			break;
		}

		mBuffer.gLightPositionAndType.set(positionAndType);
			
		Vector4 colorAndIntensity;
		colorAndIntensity.x = light->getColor().r;
		colorAndIntensity.y = light->getColor().g;
		colorAndIntensity.z = light->getColor().b;
		colorAndIntensity.w = light->getIntensity();

		mBuffer.gLightColorAndIntensity.set(colorAndIntensity);

		Vector3 spotAngles;
		spotAngles.x = light->getSpotAngle().valueRadians();
		spotAngles.y = cos(spotAngles.x);
		spotAngles.z = 1.0f / (Math::cos(light->getSpotFalloffAngle()) - spotAngles.y);

		mBuffer.gLightSpotAngles.set(spotAngles);

		mBuffer.gLightDirection.set(light->getRotation().zAxis());

		Vector4 lightGeometry;
		lightGeometry.x = light->getType() == LightType::Spot ? (float)LightCore::LIGHT_CONE_NUM_SIDES : 0;
		lightGeometry.y = (float)LightCore::LIGHT_CONE_NUM_SLICES;
		lightGeometry.z = light->getBounds().getRadius();

		float coneRadius = Math::sin(light->getSpotAngle()) * light->getRange();
		lightGeometry.w = coneRadius;

		mBuffer.gLightGeometry.set(lightGeometry);

		Matrix4 transform = Matrix4::TRS(light->getPosition(), light->getRotation(), Vector3::ONE);
		mBuffer.gMatConeTransform.set(transform);
	}

	const SPtr<GpuParamBlockBufferCore>& PerLightParams::getBuffer() const
	{
		return mBuffer.getBuffer();
	}
	
	DirectionalLightMat::DirectionalLightMat()
	{
		mMaterial->setParamBlockBuffer("PerLight", mParams.getBuffer());

		auto& texParams = mMaterial->getShader()->getTextureParams();
		for (auto& entry : texParams)
		{
			if (entry.second.rendererSemantic == RPS_GBufferA)
				mGBufferA = mMaterial->getParamTexture(entry.second.name);
			else if (entry.second.rendererSemantic == RPS_GBufferB)
				mGBufferB = mMaterial->getParamTexture(entry.second.name);
			else if (entry.second.rendererSemantic == RPS_GBufferDepth)
				mGBufferDepth = mMaterial->getParamTexture(entry.second.name);
		}
	}

	void DirectionalLightMat::setStaticParameters(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBufferCore>& perCamera)
	{
		mGBufferA.set(gbuffer->getTextureA());
		mGBufferB.set(gbuffer->getTextureB());
		mGBufferDepth.set(gbuffer->getTextureDepth());

		mMaterial->setParamBlockBuffer("PerCamera", perCamera);
	}

	void DirectionalLightMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}

	PointLightMat::PointLightMat()
	{
		mMaterial->setParamBlockBuffer("PerLight", mParams.getBuffer());

		auto& texParams = mMaterial->getShader()->getTextureParams();
		for (auto& entry : texParams)
		{
			if (entry.second.rendererSemantic == RPS_GBufferA)
				mGBufferA = mMaterial->getParamTexture(entry.second.name);
			else if (entry.second.rendererSemantic == RPS_GBufferB)
				mGBufferB = mMaterial->getParamTexture(entry.second.name);
			else if (entry.second.rendererSemantic == RPS_GBufferDepth)
				mGBufferDepth = mMaterial->getParamTexture(entry.second.name);
		}
	}

	void PointLightMat::setStaticParameters(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBufferCore>& perCamera)
	{
		mGBufferA.set(gbuffer->getTextureA());
		mGBufferB.set(gbuffer->getTextureB());
		mGBufferDepth.set(gbuffer->getTextureDepth());

		mMaterial->setParamBlockBuffer("PerCamera", perCamera);
	}

	void PointLightMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}
}