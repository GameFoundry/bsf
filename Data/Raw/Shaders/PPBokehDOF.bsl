#include "$ENGINE$\DepthOfFieldCommon.bslinc"
#include "$ENGINE$\PerCameraData.bslinc"

shader PPBokehDOF
{
	mixin DepthOfFieldCommon;
	mixin PerCameraData;

	depth
	{
		write = false;
		read = false;
	};
	
	blend
	{
		target	
		{
			enabled = true;
			color = { one, one, add };
			alpha = { one, one, add };
		};
	};

	code
	{
		cbuffer Params
		{
			uint2 gTileCount;
			float2 gInvInputSize;
			float2 gInvOutputSize;
			float gAdaptiveThresholdColor;
			float gAdaptiveThresholdCOC;
			float2 gBokehSize;
			uint gLayerPixelOffset;
		};
		
		Texture2D gInputTex;
		SamplerState gInputSampler
		{
			AddressU = CLAMP;
			AddressV = CLAMP;
			AddressW = CLAMP;
		};
	
		struct VStoFS
		{
			float4 position : SV_POSITION;
			noperspective float2 uv0 : TEXCOORD0;
			nointerpolation float4 color : TEXCOORD1;
		};

		struct VertexInput
		{
			uint vertexId : SV_VertexID;
			uint instanceId : SV_InstanceID;
			float2 uv0 : TEXCOORD0;
		};
		
		VStoFS vsmain(VertexInput input)
		{
			uint vid = input.vertexId;
			uint iid = input.instanceId;
		
			// We group quads in tiles for perfomance reasons (less instances)
			// Additionally, we group the quads within the tiles into 2x2 blocks for adaptive rendering
			// (Generally a tile is 8 quads, meaning 2 blocks per tile)
			uint quadIdxInTile = vid / 4;
			uint quadIdxInBlock = quadIdxInTile % 4;
			
			// Find the top-left position of the current block, in pixels
			uint blocksPerTile = QUADS_PER_TILE / 4;
			
			// TODO - Since the way we're sampling below, should this be the block center?
			// - Then again maybe not, because the offset we apply to blockPos below seems similar?
			uint blocksInRow = gTileCount.x / (blocksPerTile * 2);
			float2 blockPos = 
				float2(iid % blocksInRow, iid / blocksInRow)
				* uint2(blocksPerTile * 2, 2) // Each tile is 2 pixels (quads) high, with N blocks with 2 pixels (quads) wide
				+ uint2((quadIdxInTile/4) * 2, 0); // Each block has N quads laid out in a row, and each block is 2 quads wide
			
			// Color in .rgb, linear depth in .a
			float4 samples[4];
			samples[0] = gInputTex.SampleLevel(gInputSampler, (blockPos + float2(-0.5f, -0.5f)) * gInvInputSize, 0);
			samples[1] = gInputTex.SampleLevel(gInputSampler, (blockPos + float2( 0.5f, -0.5f)) * gInvInputSize, 0);
			samples[2] = gInputTex.SampleLevel(gInputSampler, (blockPos + float2(-0.5f,  0.5f)) * gInvInputSize, 0);
			samples[3] = gInputTex.SampleLevel(gInputSampler, (blockPos + float2( 0.5f,  0.5f)) * gInvInputSize, 0);

			float4 minSamples = min(min(samples[0], samples[1]), min(samples[2], samples[3]));
			float4 maxSamples = max(max(samples[0], samples[1]), max(samples[2], samples[3]));
			
			bool needSeparateQuads = false;
			
			// Are the colors too different?
			float3 colorDiff = maxSamples.rgb - minSamples.rgb;
			if(dot(colorDiff, 1) > gAdaptiveThresholdColor)
				needSeparateQuads = true;
			
			// Do the samples fall on different layers?
			int minLayer = minSamples.a < gFocalPlaneDistance ? 0 :
				(minSamples.a > (gFocalPlaneDistance + gFocalLength) ? 2 : 1);
				
			int maxLayer = maxSamples.a < gFocalPlaneDistance ? 0 :
				(maxSamples.a > (gFocalPlaneDistance + gFocalLength) ? 2 : 1);

			if(minLayer != maxLayer)
				needSeparateQuads = true;

			// Don't skip small quads
			float avgDepth = (minSamples.a + maxSamples.a) * 0.5f;
			float avgCOC = circleOfConfusionPhysical(avgDepth);
			if(avgCOC < gAdaptiveThresholdCOC)
				needSeparateQuads = true;
			
			float4 colorAndDepth;
			if(needSeparateQuads)
			{
				colorAndDepth = samples[quadIdxInBlock];
				blockPos += float2(quadIdxInBlock % 2, quadIdxInBlock / 2) - 0.5f;
			}
			else
				colorAndDepth = (samples[0] + samples[1] + samples[2] + samples[3]) * 0.25f;

			float sceneDepth = colorAndDepth.a;
			float coc = circleOfConfusionPhysical(sceneDepth);

			// 2 pixel minimum size
			float2 cocPixelSize = max(coc * gBokehSize.xy, 2.0f);

			float4 color = float4(colorAndDepth.rgb, 1);
			float2 layer = computeLayerContributions(sceneDepth);

			color *= (sceneDepth < gFocalPlaneDistance) ? layer.r : layer.g;

			if(!needSeparateQuads)
			{
				// Make other three quads zero-sized
				if(quadIdxInBlock != 0)
					cocPixelSize = 0;
			}
			else
				color *= 0.25f;

			// Offset so we render both near and far fields in the same texture but with an offset
			float vertOffset = sceneDepth < gFocalPlaneDistance ? gLayerPixelOffset : 0;

			// Determine location of the vertex within the current quad
			uint vertexIdxInQuad = vid % 4;				
			float2 localPos = float2(vertexIdxInQuad % 2, vertexIdxInQuad / 2);
			
			// TODO - UV needs flip on OpenGL?
			float2 ndcPos = UVToNDC((blockPos + (localPos - 0.5f) * cocPixelSize + float2(0, vertOffset)) * gInvOutputSize);
			
			VStoFS output;			
			output.position = float4(ndcPos, 0, 1);
			output.uv0 = input.uv0;
			output.color = color;

			return output;
		}			

		Texture2D gBokehTex;
		SamplerState gBokehSampler;
		
		float4 fsmain(VStoFS input) : SV_Target0
		{
			return gBokehTex.Sample(gBokehSampler, input.uv0).r * input.color;
		}	
	};
};