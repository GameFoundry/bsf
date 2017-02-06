#include "$ENGINE$\BasePass.bslinc"
#include "$ENGINE$\LightingCommon.bslinc"

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
			
			StructuredBuffer<LightData> gLights : register(t2);
		
			cbuffer LightParams : register(b4)
			{
				// Offsets at which specific light types begin in gLights buffer
				// Assumed directional lights start at 0
				// x - offset to point lights, y - offset to spot lights, z - total number of lights
				uint3 gLightOffsets;
			}
			
			cbuffer MaterialParams : register(b5)
			{
				float gOpacity;
			}
			
			float4 main(in VStoFS input) : SV_Target0
			{
				float3 normal = normalize(gNormalTex.Sample(gNormalSamp, input.uv0) * 2.0f - float3(1, 1, 1));
				float3 worldNormal = calcWorldNormal(input, normal);
			
				SurfaceData surfaceData;
				surfaceData.albedo = gAlbedoTex.Sample(gAlbedoSamp, input.uv0);
				surfaceData.worldNormal.xyz = worldNormal;
				
				float3 lightAccumulator = 0;
				for(uint i = 0; i < gLightOffsets[0]; ++i)
					lightAccumulator += getDirLightContibution(surfaceData, gLights[i]);
				
				for (uint i = gLightOffsets[0]; i < gLightOffsets[1]; ++i)
					lightAccumulator += getPointLightContribution(input.worldPosition, surfaceData, gLights[i]);

				for(uint i = gLightOffsets[1]; i < gLightOffsets[2]; ++i)
					lightAccumulator += getSpotLightContribution(input.worldPosition, surfaceData, gLights[i]);
				
				float3 diffuse = surfaceData.albedo.xyz / PI;
				return float4(diffuse * lightAccumulator, gOpacity); // TODO - Add better lighting model later
			}	
		};
	};
};

Technique 
 : inherits("LightingCommon")
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
		
			layout(binding = 4) uniform sampler2D gAlbedoTex;
			layout(binding = 5) uniform sampler2D gNormalTex;

			layout(std430, binding = 6) buffer gLights
			{
				LightData[] gLightsData;
			};
						
			layout(binding = 7, std140) uniform LightParams
			{
				// Offsets at which specific light types begin in gLights buffer
				// Assumed directional lights start at 0
				// x - offset to point lights, y - offset to spot lights, z - total number of lights
				uvec3 gLightOffsets;
			};
			
			layout(binding = 8, std140) uniform MaterialParams
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
				
				vec3 lightAccumulator = vec3(0, 0, 0);
				for(uint i = 0; i < gLightOffsets[0]; ++i)
				{
					LightData lightData = gLightsData[i];
					lightAccumulator += getDirLightContibution(surfaceData, lightData);
				}
				
				for (uint i = gLightOffsets[0]; i < gLightOffsets[1]; ++i)
				{
					LightData lightData = gLightsData[i];
					lightAccumulator += getPointLightContribution(worldPosition, surfaceData, lightData);
				}

				for(uint i = gLightOffsets[1]; i < gLightOffsets[2]; ++i)
				{
					LightData lightData = gLightsData[i];
					lightAccumulator += getSpotLightContribution(worldPosition, surfaceData, lightData);
				}
				
				vec3 diffuse = surfaceData.albedo.xyz / PI; // TODO - Add better lighting model later
				fragColor = vec4(diffuse * lightAccumulator, gOpacity); 
			}	
		};
	};
};

#include "$ENGINE$\Surface.bslinc"