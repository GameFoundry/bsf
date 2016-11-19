Parameters =
{
	mat4x4 	worldTransform;
	float	invViewportWidth;
	float	invViewportHeight;
	
	Sampler2D	mainTexSamp : alias("mainTexture");
	Texture2D	mainTexture;
	
	color		tint;
};

Technique =
{
	Language = "HLSL11";
	
	Pass =
	{
		Target = 
		{
			Blend = true;
			Color = { SRCA, SRCIA, ADD };
			WriteMask = RGB;
		};	
	
		DepthRead = false;
		DepthWrite = false;
		
		Vertex =
		{
			float invViewportWidth;
			float invViewportHeight;
			float4x4 worldTransform;

			void main(
				in float3 inPos : POSITION,
				in float2 uv : TEXCOORD0,
				out float4 oPosition : SV_Position,
				out float2 oUv : TEXCOORD0)
			{
				float4 tfrmdPos = mul(worldTransform, float4(inPos.xy, 0, 1));

				float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);
				float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);

				oPosition = float4(tfrmdX, tfrmdY, 0, 1);
				oUv = uv;
			}
		};
		
		Fragment =
		{
			SamplerState mainTexSamp : register(s0);
			Texture2D mainTexture : register(t0);
			float4 tint;

			float4 main(in float4 inPos : SV_Position, float2 uv : TEXCOORD0) : SV_Target
			{
				float4 color = float4(tint.rgb, mainTexture.Sample(mainTexSamp, uv).r * tint.a);
				return color;
			}
		};
	};
};

Technique =
{
	Language = "GLSL";
	
	Pass =
	{
		Target = 
		{
			Blend = true;
			Color = { SRCA, SRCIA, ADD };
			WriteMask = RGB;
		};
		
		DepthRead = false;
		DepthWrite = false;
		
		Vertex =
		{
			layout (binding = 0) uniform VertUBO
			{
				float invViewportWidth;
				float invViewportHeight;
				mat4 worldTransform;
			};

			layout (location = 0) in vec3 bs_position;
			layout (location = 1) in vec2 bs_texcoord0;
			
			layout (location = 0) out vec2 texcoord0;

			out gl_PerVertex
			{
				vec4 gl_Position;
			};
			
			void main()
			{
				vec4 tfrmdPos = worldTransform * vec4(bs_position.xy, 0, 1);

				float tfrmdX = -1.0f + (tfrmdPos.x * invViewportWidth);
				float tfrmdY = 1.0f - (tfrmdPos.y * invViewportHeight);

				gl_Position = vec4(tfrmdX, tfrmdY, 0, 1);
				texcoord0 = bs_texcoord0;
			}
		};
		
		Fragment =
		{
			layout (binding = 1) uniform FragUBO
			{
				vec4 tint;
			};		
		
			layout (binding = 2) uniform sampler2D mainTexture;
			
			layout (location = 0) in vec2 texcoord0;
			layout (location = 0) out vec4 fragColor;

			void main()
			{
				vec4 color = vec4(tint.rgb, texture2D(mainTexture, texcoord0.st).r * tint.a);
				fragColor = color;
			}
		};
	};
};