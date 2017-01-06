#include "$ENGINE$\PerCameraData.bslinc"

Parameters =
{
	SamplerCUBE 	gSkySamp : alias("gSkyTex");
	TextureCUBE  	gSkyTex;
};

Technique : inherits("PerCameraData") =
{
	Language = "HLSL11";
	
	Pass =
	{
		Cull = CW;
		CompareFunc = LTE;
		DepthWrite = false;
		
		Vertex =
		{
			void main(
				in float3 inPos : POSITION,
				out float4 oPosition : SV_Position,
				out float3 oDir : TEXCOORD0)
			{
				float4 pos = mul(gMatViewProj, float4(inPos.xyz + gViewOrigin, 1));
			
				// Set Z = W so that final depth is 1.0f and it renders behind everything else
				oPosition = pos.xyww;
				oDir = inPos;
			}
		};
		
		Fragment =
		{
			TextureCube gSkyTex : register(t0);
			SamplerState gSkySamp : register(s0);
		
			float4 main(
				in float4 inPos : SV_Position, 
				in float3 dir : TEXCOORD0) : SV_Target
			{
				return gSkyTex.Sample(gSkySamp, dir);
			}
		};	
	};
};

Technique : inherits("PerCameraData") =
{
	Language = "GLSL";
	
	Pass =
	{
		Cull = CW;
		CompareFunc = LTE;
		DepthWrite = false;
		
		Vertex = 
		{
			layout(location = 0) in vec3 bs_position;
			layout(location = 0) out vec3 dir;	
		
			out gl_PerVertex
			{
				vec4 gl_Position;
			};
		
			void main()
			{
				vec4 pos = gMatViewProj * vec4(bs_position.xyz + gViewOrigin, 1);
			
				// Set Z = W so that final depth is 1.0f and it renders behind everything else
				gl_Position = pos.xyww;
				dir = bs_position;
			}
		};
	
		Fragment =
		{
			layout(location = 0) in vec3 dir;		
			layout(binding = 1) uniform samplerCube gSkyTex;
			layout(location = 0) out vec4 fragColor;
			
			void main()
			{
				fragColor = texture(gSkyTex, dir);
			}	
		};
	};
};