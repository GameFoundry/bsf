#include "$ENGINE$\DeferredLightCommon.bslinc"

Technique : inherits("DeferredLightCommon") =
{
	Pass =
	{
		DepthRead = false;
	
		Common = 
		{
			struct VStoFS
			{
				float4 position : SV_POSITION;
				float2 uv0 : TEXCOORD0;
				float3 screenDir : TEXCOORD1;
			};
		};
	
		Vertex =
		{
			struct VertexInput
			{
				float2 screenPos : POSITION;
				float2 uv0 : TEXCOORD0;
			};
			
			VStoFS main(VertexInput input)
			{
				VStoFS output;
			
				output.position = float4(input.screenPos, 0, 1);
				output.uv0 = input.uv0;
				output.screenDir = mul(gMatInvProj, float4(input.screenPos, 1, 0)).xyz - gViewOrigin.xyz;
			
				return output;
			}			
		};
		
		Fragment = 
		{
			float4 main(VStoFS input) : SV_Target0
			{
				uint2 pixelPos = (uint2)(input.uv0 * (float2)gViewportRectangle.zw - ((float2)gViewportRectangle.xy + 0.5f));
				
				#if MSAA_COUNT > 1
				SurfaceData surfaceData = getGBufferData(pixelPos, sampleIdx);
				#else
				SurfaceData surfaceData = getGBufferData(pixelPos);
				#endif			
			
				if(surfaceData.worldNormal.w > 0.0f)
				{
					float3 cameraDir = normalize(input.screenDir);
					float3 worldPosition = input.screenDir * surfaceData.depth + gViewOrigin;
					
					float3 V = normalize(gViewOrigin - worldPosition);
					float3 N = surfaceData.worldNormal.xyz;
					float3 R = 2 * dot(V, N) * N - V;

					float roughness2 = max(surfaceData.roughness, 0.08f);
					roughness2 *= roughness2;
					
					LightData lightData = getLightData();
					return float4(getLuminanceDirectional(lightData, worldPosition, V, R, surfaceData), 1.0f);
				}
				else
					return float4(0.0f, 0.0f, 0.0f, 0.0f);
			}
		};
	};
};