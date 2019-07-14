//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Reflection/BsRTTIType.h"
#include "Renderer/BsRenderSettings.h"

namespace bs
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_CORE_EXPORT AutoExposureSettingsRTTI : public RTTIType <AutoExposureSettings, IReflectable, AutoExposureSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(histogramLog2Min, 0)
			BS_RTTI_MEMBER_PLAIN(histogramLog2Max, 1)
			BS_RTTI_MEMBER_PLAIN(histogramPctLow, 2)
			BS_RTTI_MEMBER_PLAIN(histogramPctHigh, 3)
			BS_RTTI_MEMBER_PLAIN(minEyeAdaptation, 4)
			BS_RTTI_MEMBER_PLAIN(maxEyeAdaptation, 5)
			BS_RTTI_MEMBER_PLAIN(eyeAdaptationSpeedUp, 6)
			BS_RTTI_MEMBER_PLAIN(eyeAdaptationSpeedDown, 7)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "AutoExposureSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_AutoExposureSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<AutoExposureSettings>();
		}
	};

	class BS_CORE_EXPORT TonemappingSettingsRTTI : public RTTIType <TonemappingSettings, IReflectable, TonemappingSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(filmicCurveShoulderStrength, 0)
			BS_RTTI_MEMBER_PLAIN(filmicCurveLinearStrength, 1)
			BS_RTTI_MEMBER_PLAIN(filmicCurveLinearAngle, 2)
			BS_RTTI_MEMBER_PLAIN(filmicCurveToeStrength, 3)
			BS_RTTI_MEMBER_PLAIN(filmicCurveToeNumerator, 4)
			BS_RTTI_MEMBER_PLAIN(filmicCurveToeDenominator, 5)
			BS_RTTI_MEMBER_PLAIN(filmicCurveLinearWhitePoint, 6)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "TonemappingSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_TonemappingSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<TonemappingSettings>();
		}
	};

	class BS_CORE_EXPORT WhiteBalanceSettingsRTTI : public RTTIType <WhiteBalanceSettings, IReflectable, WhiteBalanceSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(temperature, 0)
			BS_RTTI_MEMBER_PLAIN(tint, 1)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "WhiteBalanceSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_WhiteBalanceSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<WhiteBalanceSettings>();
		}
	};

	class BS_CORE_EXPORT ColorGradingSettingsRTTI : public RTTIType <ColorGradingSettings, IReflectable, ColorGradingSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(saturation, 0)
			BS_RTTI_MEMBER_PLAIN(contrast, 1)
			BS_RTTI_MEMBER_PLAIN(gain, 2)
			BS_RTTI_MEMBER_PLAIN(offset, 3)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "ColorGradingSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ColorGradingSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ColorGradingSettings>();
		}
	};

	class BS_CORE_EXPORT DepthOfFieldSettingsRTTI : public RTTIType <DepthOfFieldSettings, IReflectable, DepthOfFieldSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(enabled, 0)
			BS_RTTI_MEMBER_PLAIN(focalDistance, 1)
			BS_RTTI_MEMBER_PLAIN(focalRange, 2)
			BS_RTTI_MEMBER_PLAIN(nearTransitionRange, 3)
			BS_RTTI_MEMBER_PLAIN(farTransitionRange, 4)
			BS_RTTI_MEMBER_PLAIN(nearBlurAmount, 5)
			BS_RTTI_MEMBER_PLAIN(farBlurAmount, 6)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "DepthOfFieldSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_DepthOfFieldSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<DepthOfFieldSettings>();
		}
	};

	class BS_CORE_EXPORT AmbientOcclusionSettingsRTTI : public RTTIType <AmbientOcclusionSettings, IReflectable, AmbientOcclusionSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(enabled, 0)
			BS_RTTI_MEMBER_PLAIN(radius, 1)
			BS_RTTI_MEMBER_PLAIN(bias, 2)
			BS_RTTI_MEMBER_PLAIN(fadeRange, 3)
			BS_RTTI_MEMBER_PLAIN(fadeDistance, 4)
			BS_RTTI_MEMBER_PLAIN(intensity, 5)
			BS_RTTI_MEMBER_PLAIN(power, 6)
			BS_RTTI_MEMBER_PLAIN(quality, 7)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "AmbientOcclusionSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_AmbientOcclusionSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<AmbientOcclusionSettings>();
		}
	};

	class BS_CORE_EXPORT ScreenSpaceReflectionsSettingsRTTI : public RTTIType <ScreenSpaceReflectionsSettings, IReflectable, ScreenSpaceReflectionsSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(enabled, 0)
			BS_RTTI_MEMBER_PLAIN(intensity, 1)
			BS_RTTI_MEMBER_PLAIN(maxRoughness, 2)
			BS_RTTI_MEMBER_PLAIN(quality, 3)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "ScreenSpaceReflectionsSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ScreenSpaceReflectionsSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ScreenSpaceReflectionsSettings>();
		}
	};

	class BS_CORE_EXPORT BloomSettingsRTTI : public RTTIType <BloomSettings, IReflectable, BloomSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(enabled, 0)
			BS_RTTI_MEMBER_PLAIN(quality, 1)
			BS_RTTI_MEMBER_PLAIN(threshold, 2)
			BS_RTTI_MEMBER_PLAIN(intensity, 3)
			BS_RTTI_MEMBER_PLAIN(tint, 4)
			BS_RTTI_MEMBER_PLAIN(filterSize, 5)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "BloomSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_BloomSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<BloomSettings>();
		}
	};

	class BS_CORE_EXPORT ScreenSpaceLensFlareSettingsRTTI :
	public RTTIType <ScreenSpaceLensFlareSettings, IReflectable, ScreenSpaceLensFlareSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(enabled, 0)
			BS_RTTI_MEMBER_PLAIN(downsampleCount, 1)
			BS_RTTI_MEMBER_PLAIN(threshold, 2)
			BS_RTTI_MEMBER_PLAIN(ghostCount, 3)
			BS_RTTI_MEMBER_PLAIN(ghostSpacing, 4)
			BS_RTTI_MEMBER_PLAIN(brightness, 5)
			BS_RTTI_MEMBER_PLAIN(filterSize, 6)
			BS_RTTI_MEMBER_PLAIN(halo, 7)
			BS_RTTI_MEMBER_PLAIN(haloAspectRatio, 8)
			BS_RTTI_MEMBER_PLAIN(haloRadius, 9)
			BS_RTTI_MEMBER_PLAIN(haloThickness, 10)
			BS_RTTI_MEMBER_PLAIN(haloThreshold, 11)
			BS_RTTI_MEMBER_PLAIN(chromaticAberration, 12)
			BS_RTTI_MEMBER_PLAIN(chromaticAberrationOffset, 13)
			BS_RTTI_MEMBER_PLAIN(bicubicUpsampling, 14)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "ScreenSpaceLensFlareSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ScreenSpaceLensFlareSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ScreenSpaceLensFlareSettings>();
		}
	};

	class BS_CORE_EXPORT ShadowSettingsRTTI : public RTTIType <ShadowSettings, IReflectable, ShadowSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(directionalShadowDistance, 0)
			BS_RTTI_MEMBER_PLAIN(numCascades, 1)
			BS_RTTI_MEMBER_PLAIN(cascadeDistributionExponent, 2)
			BS_RTTI_MEMBER_PLAIN(shadowFilteringQuality, 3)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "ShadowSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ShadowSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<ShadowSettings>();
		}
	};

	class BS_CORE_EXPORT RenderSettingsRTTI : public RTTIType <RenderSettings, IReflectable, RenderSettingsRTTI>
	{
	private:
		BS_BEGIN_RTTI_MEMBERS
			BS_RTTI_MEMBER_PLAIN(enableAutoExposure, 0)
			BS_RTTI_MEMBER_REFL(autoExposure, 1)
			BS_RTTI_MEMBER_PLAIN(enableTonemapping, 2)
			BS_RTTI_MEMBER_REFL(tonemapping, 3)
			BS_RTTI_MEMBER_REFL(whiteBalance, 4)
			BS_RTTI_MEMBER_REFL(colorGrading, 5)
			BS_RTTI_MEMBER_PLAIN(exposureScale, 6)
			BS_RTTI_MEMBER_PLAIN(gamma, 7)
			BS_RTTI_MEMBER_REFL(depthOfField, 8)
			BS_RTTI_MEMBER_PLAIN(enableFXAA, 9)
			BS_RTTI_MEMBER_REFL(ambientOcclusion, 10)
			BS_RTTI_MEMBER_REFL(screenSpaceReflections, 11)
			BS_RTTI_MEMBER_PLAIN(enableHDR, 12)
			BS_RTTI_MEMBER_PLAIN(enableLighting, 13)
			BS_RTTI_MEMBER_PLAIN(enableShadows, 14)
			BS_RTTI_MEMBER_PLAIN(overlayOnly, 15)
			BS_RTTI_MEMBER_PLAIN(enableIndirectLighting, 16)
			BS_RTTI_MEMBER_REFL(shadowSettings, 17)
			BS_RTTI_MEMBER_PLAIN(enableSkybox, 18)
			BS_RTTI_MEMBER_REFL(bloom, 19)
			BS_RTTI_MEMBER_REFL(screenSpaceLensFlare, 20)
		BS_END_RTTI_MEMBERS

	public:
		const String& getRTTIName() override
		{
			static String name = "RenderSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_RenderSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<RenderSettings>();
		}
	};

	/** @} */
	/** @endcond */
}
