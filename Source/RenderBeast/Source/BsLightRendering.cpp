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
	LightRenderingParams::LightRenderingParams(const SPtr<MaterialCore>& material)
	{
		auto& texParams = material->getShader()->getTextureParams();
		SPtr<GpuParamsCore> fragParams = material->getGpuParams(0, GPT_FRAGMENT_PROGRAM);
		for (auto& entry : texParams)
		{
			if (entry.second.rendererSemantic == RPS_GBufferA)
				fragParams->getTextureParam(entry.second.name, mGBufferA);
			else if (entry.second.rendererSemantic == RPS_GBufferB)
				fragParams->getTextureParam(entry.second.name, mGBufferB);
			else if (entry.second.rendererSemantic == RPS_GBufferDepth)
				fragParams->getTextureParam(entry.second.name, mGBufferDepth);
		}

		mParamDescFP = &fragParams->getParamDesc();
		mPerLightBlockDescFP = fragParams->getParamBlockDesc("PerLight");
		mPerCameraBlockDescFP = fragParams->getParamBlockDesc("PerCamera");

		SPtr<GpuParamsCore> vertParams = material->getGpuParams(0, GPT_VERTEX_PROGRAM);
		mParamDescVP = &vertParams->getParamDesc();
		mPerLightBlockDescVP = vertParams->getParamBlockDesc("PerLight");
		mPerCameraBlockDescVP = vertParams->getParamBlockDesc("PerCamera");
	}

	void LightRenderingParams::setStaticParameters(RenderAPICore& rapi, const SPtr<RenderTargets>& gbuffer,
		const SPtr<GpuParamBlockBufferCore>& perCamera)
	{
		rapi.setTexture(GPT_FRAGMENT_PROGRAM, mGBufferA.getDesc().slot, gbuffer->getTextureA());
		rapi.setTexture(GPT_FRAGMENT_PROGRAM, mGBufferB.getDesc().slot, gbuffer->getTextureB());
		rapi.setTexture(GPT_FRAGMENT_PROGRAM, mGBufferDepth.getDesc().slot, gbuffer->getTextureDepth());

		rapi.setSamplerState(GPT_FRAGMENT_PROGRAM, 0, SamplerStateCore::getDefault());
		rapi.setSamplerState(GPT_FRAGMENT_PROGRAM, 1, SamplerStateCore::getDefault());
		rapi.setSamplerState(GPT_FRAGMENT_PROGRAM, 2, SamplerStateCore::getDefault());

		rapi.setParamBuffer(GPT_FRAGMENT_PROGRAM, mPerLightBlockDescFP->slot, mBuffer.getBuffer(), *mParamDescFP);
		rapi.setParamBuffer(GPT_FRAGMENT_PROGRAM, mPerCameraBlockDescFP->slot, perCamera, *mParamDescFP);

		if(mPerLightBlockDescVP != nullptr)
			rapi.setParamBuffer(GPT_VERTEX_PROGRAM, mPerLightBlockDescVP->slot, mBuffer.getBuffer(), *mParamDescVP);

		rapi.setParamBuffer(GPT_VERTEX_PROGRAM, mPerCameraBlockDescVP->slot, perCamera, *mParamDescVP);
	}

	void LightRenderingParams::setParameters(const LightCore* light)
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

		mBuffer.flushToGPU();
	}

	const SPtr<GpuParamBlockBufferCore>& LightRenderingParams::getBuffer() const
	{
		return mBuffer.getBuffer();
	}
	
	DirectionalLightMat::DirectionalLightMat()
		:mParams(mMaterial)
	{
		
	}

	void DirectionalLightMat::_initDefines(ShaderDefines& defines)
	{
		// Do nothing
	}

	void DirectionalLightMat::setStaticParameters(RenderAPICore& rapi, const SPtr<RenderTargets>& gbuffer, 
		const SPtr<GpuParamBlockBufferCore>& perCamera)
	{
		mParams.setStaticParameters(rapi, gbuffer, perCamera);
	}

	void DirectionalLightMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}

	PointLightInMat::PointLightInMat()
		:mParams(mMaterial)
	{

	}

	void PointLightInMat::_initDefines(ShaderDefines& defines)
	{
		// Do nothing
	}

	void PointLightInMat::setStaticParameters(RenderAPICore& rapi, const SPtr<RenderTargets>& gbuffer, 
		const SPtr<GpuParamBlockBufferCore>& perCamera)
	{
		mParams.setStaticParameters(rapi, gbuffer, perCamera);
	}

	void PointLightInMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}

	PointLightOutMat::PointLightOutMat()
		:mParams(mMaterial)
	{
		
	}

	void PointLightOutMat::_initDefines(ShaderDefines& defines)
	{
		// Do nothing
	}

	void PointLightOutMat::setStaticParameters(RenderAPICore& rapi, const SPtr<RenderTargets>& gbuffer, 
		const SPtr<GpuParamBlockBufferCore>& perCamera)
	{
		mParams.setStaticParameters(rapi, gbuffer, perCamera);
	}

	void PointLightOutMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}
}