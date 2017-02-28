#include "$ENGINE$\BasePass.bslinc"

Parameters = 
{
	Sampler2D 	samp : alias("tex");
	Texture2D 	tex;
};

Technique : base("Surface") =
{
	Language = "HLSL11";
	
	Pass = 
	{
		Fragment =
		{
			SamplerState samp : register(s0);
			Texture2D tex : register(t0);
		
			void main(
				in VStoFS input, 
				out float4 OutGBufferA : SV_Target0,
				out float4 OutGBufferB : SV_Target1,
				out float2 OutGBufferC : SV_Target2)
			{
				SurfaceData surfaceData;
				surfaceData.albedo = float4(tex.Sample(samp, input.uv0).xyz, 1.0f);
				surfaceData.worldNormal.xyz = input.tangentToWorldZ;
				surfaceData.roughness = 1.0f;
				surfaceData.metalness = 0.0f;
				
				encodeGBuffer(surfaceData, OutGBufferA, OutGBufferB, OutGBufferC);
			}	
		};
	};
};

Technique : base("Surface") =
{
	Language = "GLSL";
	
	Pass =
	{
		Fragment =
		{
			layout(location = 0) in vec2 uv0;
			layout(location = 1) in vec3 worldPosition;
			layout(location = 2) in vec3 tangentToWorldZ;
			layout(location = 3) in vec4 tangentToWorldX;			
		
			layout(binding = 4) uniform sampler2D tex;

			layout(location = 0) out vec4 fragColor[3];
			
			void main()
			{
				SurfaceData surfaceData;
				surfaceData.albedo = texture(tex, uv0);
				surfaceData.worldNormal.xyz = tangentToWorldZ;
				surfaceData.roughness = 1.0f;
				surfaceData.metalness = 0.0f;
				
				encodeGBuffer(surfaceData, fragColor[0], fragColor[1], fragColor[2]);
			}	
		};
	};
};

#include "$ENGINE$\Surface.bslinc"