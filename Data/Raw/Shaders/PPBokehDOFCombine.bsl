#include "$ENGINE$\DepthOfFieldCommon.bslinc"
#include "$ENGINE$\DepthInput.bslinc"
#include "$ENGINE$\PPBase.bslinc"
#include "$ENGINE$\PerCameraData.bslinc"

shader PPBokehDOF
{
	mixin DepthOfFieldCommon;
	mixin PPBase;
	mixin DepthInput;
	mixin PerCameraData;

	code
	{
		cbuffer Params
		{
			float2 gLayerUVScaleOffset;
		};
		
		Texture2D gUnfocusedTex;
		SamplerState gUnfocusedSampler
		{
			AddressU = CLAMP;
			AddressV = CLAMP;
			AddressW = CLAMP;
		};
		
		Texture2D gFocusedTex;
		SamplerState gFocusedSampler;
			
		float4 fsmain(VStoFS input) : SV_Target0
		{
			float2 uv = input.uv0;
			
			float3 focusedColor = gFocusedTex.Sample(gFocusedSampler, uv).rgb;
			float depth = -convertFromDeviceZ(gDepthBufferTex.SampleLevel(gDepthBufferSamp, uv, 0).r);
			
			float2 halfUV = uv;
			halfUV.y = halfUV.y * gLayerUVScaleOffset.x;
			
			float4 blurredFocusedAndFar = gUnfocusedTex.Sample(gUnfocusedSampler, halfUV);
			float4 blurredNear = gUnfocusedTex.Sample(gUnfocusedSampler, halfUV + float2(0, gLayerUVScaleOffset.y));

			float TWEAK_FACTOR_1 = 0.18f; // TODO: Expose to user code?
			float TWEAK_FACTOR_2 = 5; // TODO: Expose to user code?
			
			float farMask = saturate(blurredFocusedAndFar.a * TWEAK_FACTOR_1);
			float focusedMask = 1.0f - saturate(circleOfConfusionPhysical(depth) * TWEAK_FACTOR_2);
			float nearMask = saturate(blurredNear.a * TWEAK_FACTOR_1);

			float4 combined = 0;

			// Blend half-res far layer
			combined.rgb = lerp(focusedColor, blurredFocusedAndFar.rgb / max(blurredFocusedAndFar.a, 0.001f), farMask);

			// Blend full-res in-focus area
			combined = lerp(combined, float4(focusedColor, 1), focusedMask);

			// Blend half-res near layer
			combined = lerp(combined, float4(blurredNear.rgb / max(blurredNear.a, 0.001f), 0), nearMask);

			float weight = combined.a;
			return float4(lerp(combined.rgb, focusedColor.rgb, weight), 1.0f);
		}	
	};
};