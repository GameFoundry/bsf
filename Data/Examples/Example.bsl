#include "$ENGINE$\DeferredBasePass.bslinc"

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
		
			float4 main(
				in VStoFS input, 
				out float4 OutGBufferA : SV_Target1,
				out float4 OutGBufferB : SV_Target2) : SV_Target0
			{
				GBufferData gbufferData;
				gbufferData.albedo = float4(tex.Sample(samp, input.uv0).xyz, 1.0f);
				gbufferData.worldNormal.xyz = input.tangentToWorldZ;
				
				encodeGBuffer(gbufferData, OutGBufferA, OutGBufferB);
				
				// TODO - Just returning a simple ambient term, use better environment lighting later
				return float4(gbufferData.albedo.rgb, 1.0f) * 0.2f; 
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
			layout(location = 1) in vec3 tangentToWorldZ;
			layout(location = 2) in vec4 tangentToWorldX;			
		
			layout(binding = 4) uniform sampler2D tex;

			void main()
			{
				GBufferData gbufferData;
				gbufferData.albedo = texture(tex, uv0);
				gbufferData.worldNormal.xyz = tangentToWorldZ;
				
				encodeGBuffer(gbufferData, gl_FragData[1], gl_FragData[2]);
				
				// TODO - Just returning a simple ambient term, use better environment lighting later
				gl_FragData[0] = vec4(gbufferData.albedo.rgb, 1.0f) * 0.2f; 
			}	
		};
	};
};

#include "$ENGINE$\Surface.bslinc"