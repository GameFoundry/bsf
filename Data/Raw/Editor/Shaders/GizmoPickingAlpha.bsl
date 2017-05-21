technique GizmoPickingAlpha
{
	raster
	{
		scissor = true;
	};
	
	code
	{
		cbuffer Uniforms
		{
			float4x4 	gMatViewProj;
			float		gAlphaCutoff;
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

		float4 fsmain(in float4 inPos : SV_Position, 
					   in float2 uv : TEXCOORD0,
					   in float4 inColor : COLOR0) : SV_Target
		{
			float4 color = gMainTexture.Sample(gMainTexSamp, uv);
			if(color.a < gAlphaCutoff)
				discard;
			
			return inColor;
		}
	};
};