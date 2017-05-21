technique GizmoPicking
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
			in float4 inColor : COLOR0,
			out float4 oPosition : SV_Position,
			out float4 oColor : COLOR0)
		{
			oPosition = mul(gMatViewProj, float4(inPos.xyz, 1));
			oColor = inColor;
		}

		float4 fsmain(in float4 inPos : SV_Position, in float4 inColor : COLOR0) : SV_Target
		{
			return inColor;
		}
	};
};