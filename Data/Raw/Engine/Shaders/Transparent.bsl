#include "$ENGINE$\BasePass.bslinc"
#include "$ENGINE$\LightingCommon.bslinc"
#include "$ENGINE$\LightGridCommon.bslinc"

Parameters =
{
	Sampler2D 	gAlbedoSamp : alias("gAlbedoTex");
	Sampler2D 	gNormalSamp : alias("gNormalTex");
	
	Texture2D 	gAlbedoTex;
	Texture2D	gNormalTex = "normal";
	
	float		gOpacity   = 1.0f;
};

Blocks =
{
	Block MaterialParams;
};

Transparent = true;

Technique 
 : inherits("LightingCommon")
 : inherits("LightGridCommon")
 : base("Surface") =
{
	Language = "HLSL11";
	
	Pass =
	{
		DepthWrite = false;
	
		Target = 
		{
			Blend = true;
			Color = { SRCA, SRCIA, ADD };
		};	
	
		Fragment =
		{
			SamplerState gAlbedoSamp : register(s0);
			SamplerState gNormalSamp : register(s1);
			
			Texture2D gAlbedoTex : register(t0);
			Texture2D gNormalTex : register(t1);
			
			Buffer<uint3> gGridOffsetsAndSize : register(t2);
			Buffer<uint> gGridLightIndices : register(t3);
			StructuredBuffer<LightData> gLights : register(t4);

			cbuffer MaterialParams : register(b5)
			{
				float gOpacity;
			}
			
			float4 main(in VStoFS input) : SV_Target0
			{
				float3 normal = normalize(gNormalTex.Sample(gNormalSamp, input.uv0).xyz * 2.0f - float3(1, 1, 1));
				float3 worldNormal = calcWorldNormal(input, normal);
			
				SurfaceData surfaceData;
				surfaceData.albedo = gAlbedoTex.Sample(gAlbedoSamp, input.uv0);
				surfaceData.worldNormal.xyz = worldNormal;
				
				float3 lightAccumulator = 0;
				
				// Directional lights
				for(uint lightIdx = 0; lightIdx < gLightOffsets[0]; ++lightIdx)
					lightAccumulator += getDirLightContibution(surfaceData, gLights[lightIdx]);
				
				uint2 pixelPos = (uint2)input.position.xy;
				uint cellIdx = calcCellIdx(pixelPos, input.position.z);
				uint3 offsetAndSize = gGridOffsetsAndSize[cellIdx];
				
				// Radial lights
				uint i = offsetAndSize.x;
				uint end = offsetAndSize.x + offsetAndSize.y;
				for(; i < end; i++)
				{
					uint lightIndex = gGridLightIndices[i];
					LightData lightData = gLights[lightIndex];
					
					lightAccumulator += getPointLightContribution(input.worldPosition, surfaceData, lightData);
				}
				
				// Spot lights
				end += offsetAndSize.z;
				for(; i < end; i++)
				{
					uint lightIndex = gGridLightIndices[i];
					LightData lightData = gLights[lightIndex];
					
					lightAccumulator += getSpotLightContribution(input.worldPosition, surfaceData, lightData);
				}
				
				lightAccumulator += surfaceData.albedo.xyz * gAmbientFactor;
				
				float3 diffuse = surfaceData.albedo.xyz / PI;
				return float4(diffuse * lightAccumulator, gOpacity); // TODO - Add better lighting model later
			}	
		};
	};
};

Technique 
 : inherits("LightingCommon")
 : inherits("LightGridCommon")
 : base("Surface") =
{
	Language = "GLSL";
	
	Pass =
	{
		DepthWrite = false;
		
		Target = 
		{
			Blend = true;
			Color = { SRCA, SRCIA, ADD };
		};
	
		Fragment =
		{
			layout(location = 0) in vec2 uv0;
			layout(location = 1) in vec3 worldPosition;
			layout(location = 2) in vec3 tangentToWorldZ;
			layout(location = 3) in vec4 tangentToWorldX;			
		
			layout(binding = 5) uniform sampler2D gAlbedoTex;
			layout(binding = 6) uniform sampler2D gNormalTex;

			layout(binding = 7) uniform usamplerBuffer gGridOffsetsAndSize;
			layout(binding = 8) uniform usamplerBuffer gGridLightIndices;
			layout(std430, binding = 9) readonly buffer gLights
			{
				LightData[] gLightsData;
			};
						
			layout(binding = 10, std140) uniform MaterialParams
			{
				float gOpacity;
			};
			
			layout(location = 0) out vec4 fragColor;
			
			void main()
			{
				vec3 normal = normalize(texture(gNormalTex, uv0).xyz * 2.0f - vec3(1, 1, 1));
				vec3 worldNormal = calcWorldNormal(tangentToWorldZ, tangentToWorldX, normal);
			
				SurfaceData surfaceData;
				surfaceData.albedo = texture(gAlbedoTex, uv0);
				surfaceData.worldNormal.xyz = worldNormal;
				
				// Directional lights
				vec3 lightAccumulator = vec3(0, 0, 0);
				for(uint i = 0; i < gLightOffsets[0]; ++i)
				{
					LightData lightData = gLightsData[i];
					lightAccumulator += getDirLightContibution(surfaceData, lightData);
				}
				
				uvec2 pixelPos = uvec2(gl_FragCoord.xy);
				int cellIdx = calcCellIdx(pixelPos, gl_FragCoord.z);
				uvec3 offsetAndSize = texelFetch(gGridOffsetsAndSize, cellIdx).xyz;
				
				// Radial lights
				int i = int(offsetAndSize.x);
				uint end = offsetAndSize.x + offsetAndSize.y;
				for(; i < end; i++)
				{
					uint lightIndex = texelFetch(gGridLightIndices, i).x;
					LightData lightData = gLightsData[lightIndex];
					
					lightAccumulator += getPointLightContribution(worldPosition, surfaceData, lightData);
				}
				
				// Spot lights
				end += offsetAndSize.z;
				for(; i < end; i++)
				{
					uint lightIndex = texelFetch(gGridLightIndices, i).x;
					LightData lightData = gLightsData[lightIndex];
					
					lightAccumulator += getSpotLightContribution(worldPosition, surfaceData, lightData);
				}
				
				lightAccumulator += surfaceData.albedo.xyz * gAmbientFactor;
				
				vec3 diffuse = surfaceData.albedo.xyz / PI; // TODO - Add better lighting model later
				fragColor = vec4(diffuse * lightAccumulator, gOpacity); 
			}	
		};
	};
};

#include "$ENGINE$\Surface.bslinc"