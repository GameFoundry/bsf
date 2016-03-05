//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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

		Radian spotAngle = Math::clamp(light->getSpotAngle() * 0.5f, Degree(1), Degree(90));
		Radian spotFalloffAngle = Math::clamp(light->getSpotFalloffAngle() * 0.5f, Degree(1), (Degree)spotAngle);

		Vector4 spotAnglesAndInvSqrdRadius;
		spotAnglesAndInvSqrdRadius.x = spotAngle.valueRadians();
		spotAnglesAndInvSqrdRadius.y = Math::cos(spotAnglesAndInvSqrdRadius.x);
		spotAnglesAndInvSqrdRadius.z = 1.0f / (Math::cos(spotFalloffAngle) - spotAnglesAndInvSqrdRadius.y);
		spotAnglesAndInvSqrdRadius.w = 1.0f / (light->getBounds().getRadius() * light->getBounds().getRadius());

		mBuffer.gLightSpotAnglesAndSqrdInvRadius.set(spotAnglesAndInvSqrdRadius);

		mBuffer.gLightDirection.set(-light->getRotation().zAxis());

		Vector4 lightGeometry;
		lightGeometry.x = light->getType() == LightType::Spot ? (float)LightCore::LIGHT_CONE_NUM_SIDES : 0;
		lightGeometry.y = (float)LightCore::LIGHT_CONE_NUM_SLICES;
		lightGeometry.z = light->getBounds().getRadius();

		float coneRadius = Math::sin(spotAngle) * light->getRange();
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

	PointLightInMat::PointLightInMat()
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

	void PointLightInMat::setStaticParameters(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBufferCore>& perCamera)
	{
		mGBufferA.set(gbuffer->getTextureA());
		mGBufferB.set(gbuffer->getTextureB());
		mGBufferDepth.set(gbuffer->getTextureDepth());

		mMaterial->setParamBlockBuffer("PerCamera", perCamera);
	}

	void PointLightInMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}

	PointLightOutMat::PointLightOutMat()
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

	void PointLightOutMat::setStaticParameters(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBufferCore>& perCamera)
	{
		mGBufferA.set(gbuffer->getTextureA());
		mGBufferB.set(gbuffer->getTextureB());
		mGBufferDepth.set(gbuffer->getTextureDepth());

		mMaterial->setParamBlockBuffer("PerCamera", perCamera);
	}

	void PointLightOutMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}
}