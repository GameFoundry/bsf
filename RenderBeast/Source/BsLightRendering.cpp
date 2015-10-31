#include "BsLightRendering.h"
#include "BsMaterial.h"
#include "BsGpuParams.h"
#include "BsLight.h"

namespace BansheeEngine
{
	/**
	 * Manipulates PerLight parameter buffer used in various shaders.
	 */
	class PerLightParamBuffer
	{
	public:
		// TODO - Doc
		void initialize()
		{
			// TODO - This is never called
			// TODO - Create the buffer (think of an automated way of doing it, creating the desc and retrieving the params.
			
			mParams->getParam("gLightPositionAndType", mParamLightPositionAndType);
			mParams->getParam("gLightColorAndIntensity", mParamLightColorAndIntensity);
			mParams->getParam("gLightSpotAngles", mParamLightSpotAngles);
			mParams->getParam("gLightDirection", mParamLightDirection);
			mParams->getParam("gLightGeometry", mParamLightGeometry);
		}

		// TODO - Doc
		void setParameters(const LightCore* light)
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

			mParamLightPositionAndType.set(positionAndType);
			
			Vector4 colorAndIntensity;
			colorAndIntensity.x = light->getColor().r;
			colorAndIntensity.y = light->getColor().g;
			colorAndIntensity.z = light->getColor().b;
			colorAndIntensity.w = light->getIntensity();

			mParamLightColorAndIntensity.set(colorAndIntensity);

			Vector2 spotAngles;
			spotAngles.x = light->getSpotFalloffAngle().valueDegrees();
			spotAngles.y = light->getSpotAngle().valueDegrees();

			mParamLightSpotAngles.set(spotAngles);

			mParamLightDirection.set(light->getRotation().zAxis());

			Vector4 lightGeometry;
			lightGeometry.x = 20; // Cone geometry sides
			lightGeometry.y = 10; // Cone geometry slices
			lightGeometry.z = light->getBounds().getRadius();

			lightGeometry.w = light->getSpotAngle().valueDegrees();

			mParamLightGeometry.set(lightGeometry);
		}

		// TODO - Doc
		SPtr<GpuParamBlockBufferCore> getBuffer()
		{
			return mParams->getParamBlockBuffer(0);
		}

		static PerLightParamBuffer instance;

	private:
		SPtr<GpuParamsCore> mParams;

		GpuParamVec4Core mParamLightPositionAndType;
		GpuParamVec4Core mParamLightColorAndIntensity;
		GpuParamVec2Core mParamLightSpotAngles;
		GpuParamVec3Core mParamLightDirection;
		GpuParamVec4Core mParamLightGeometry;
	};

	PerLightParamBuffer PerLightParamBuffer::instance;

	void DirectionalLightMat::setParameters(const LightCore* light)
	{
		PerLightParamBuffer::instance.setParameters(light);
	}

	void DirectionalLightMat::initialize()
	{
		mMaterial->setParamBlockBuffer("PerLight", PerLightParamBuffer::instance.getBuffer());
	}

	void PointLightMat::setParameters(const LightCore* light)
	{
		PerLightParamBuffer::instance.setParameters(light);
	}

	void PointLightMat::initialize()
	{
		mMaterial->setParamBlockBuffer("PerLight", PerLightParamBuffer::instance.getBuffer());
	}
}