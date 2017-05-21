technique IconGizmo
{
	pass
	{
		blend
		{
			target
			{
				enabled = true;
				color = { srcA, srcIA, add };
			};
		};
		
		depth
		{
			write = false;
			compare = lte;
		};
		
		code
		{
			cbuffer Uniforms
			{
				float4x4 	gMatViewProj;
				float4		gViewDir;
			}

			void vsmain(
				in float3 inPos : POSITION,
				in float2 uv : TEXCOORD0,
				in float4 color : COLOR0,
				out float4 oPosition : SV_Position,
				out float2 oUv : TEXCOORD0,
				out float4 oColor : COLOR0)
			{
				oPosition = mul(gMatViewProj, float4(inPos.xyz, 1));
				oUv = uv;
				oColor = color;
			}		

			SamplerState gMainTexSamp : register(s0);
			Texture2D gMainTexture : register(t0);

			float4 fsmain(
				in float4 inPos : SV_Position, 
				float2 uv : TEXCOORD0, 
				float4 color : COLOR0) : SV_Target
			{
				return color * gMainTexture.Sample(gMainTexSamp, uv);
			}		
		};
	};
	
	pass
	{
		blend
		{
			target
			{
				enabled = true;
				color = { srcA, srcIA, add };
			};
		};
		
		depth
		{
			write = false;
			compare = gt;
		};
		
		code
		{
			cbuffer Uniforms
			{
				float4x4 	gMatViewProj;
				float4		gViewDir;
			}

			void vsmain(
				in float3 inPos : POSITION,
				in float2 uv : TEXCOORD0,
				in float4 color : COLOR1,
				out float4 oPosition : SV_Position,
				out float2 oUv : TEXCOORD0,
				out float4 oColor : COLOR0)
			{
				oPosition = mul(gMatViewProj, float4(inPos.xyz, 1));
				oUv = uv;
				oColor = color;
			}		

			SamplerState gMainTexSamp : register(s0);
			Texture2D gMainTexture : register(t0);

			float4 fsmain(
				in float4 inPos : SV_Position, 
				float2 uv : TEXCOORD0, 
				float4 color : COLOR0) : SV_Target
			{
				return color * gMainTexture.Sample(gMainTexSamp, uv);
			}		
		};
	};	
};	