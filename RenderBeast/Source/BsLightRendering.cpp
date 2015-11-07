#include "BsLightRendering.h"
#include "BsMaterial.h"
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

		Vector2 spotAngles;
		spotAngles.x = light->getSpotFalloffAngle().valueDegrees();
		spotAngles.y = light->getSpotAngle().valueDegrees();

		mBuffer.gLightSpotAngles.set(spotAngles);

		mBuffer.gLightDirection.set(light->getRotation().zAxis());

		Vector4 lightGeometry;
		lightGeometry.x = 20; // Cone geometry sides
		lightGeometry.y = 10; // Cone geometry slices
		lightGeometry.z = light->getBounds().getRadius();

		lightGeometry.w = light->getSpotAngle().valueDegrees();

		mBuffer.gLightGeometry.set(lightGeometry);
	}

	const SPtr<GpuParamBlockBufferCore>& PerLightParams::getBuffer() const
	{
		return mBuffer.getBuffer();
	}
	
	void DirectionalLightMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}

	void DirectionalLightMat::initialize()
	{
		mMaterial->setParamBlockBuffer("PerLight", mParams.getBuffer());
	}

	void PointLightMat::setParameters(const LightCore* light)
	{
		mParams.setParameters(light);
	}

	void PointLightMat::initialize()
	{
		mMaterial->setParamBlockBuffer("PerLight", mParams.getBuffer());
	}
}