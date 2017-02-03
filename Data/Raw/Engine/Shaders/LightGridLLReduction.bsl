Technique
{
	Language = "HLSL11";
	
	Pass =
	{
		Compute = 
		{
			cbuffer Params : register(b0)
			{
				// Offsets at which specific light types begin in gLights buffer
				// Assumed directional lights start at 0
				// x - offset to point lights, y - offset to spot lights, z - total number of lights
				uint3 gLightOffsets;			
				uint gNumCells;
				uint3 gGridSize;
				uint gMaxNumLightsPerCell;
			}
			
			Buffer<uint> gLinkedListHeads : register(t0);
			Buffer<uint2> gLinkedList : register(t1);
			
			RWBuffer<uint> gGridDataCounter : register(u0);
			RWBuffer<uint2> gGridLightOffsetAndSize : register(u1);
			RWBuffer<uint> gGridLightIndices : register(u2);
			
			[numthreads(THREADGROUP_SIZE, THREADGROUP_SIZE, THREADGROUP_SIZE)]
			void main(
				uint3 groupId : SV_GroupID,
				uint3 groupThreadId : SV_GroupThreadID,
				uint3 dispatchThreadId : SV_DispatchThreadID)
			{
				uint2 viewportMax = gViewportRectangle.xy + gViewportRectangle.zw;

				// Ignore pixels out of valid range
				if (all(dispatchThreadId.xy >= viewportMax))
					return;
					
				uint maxNumLinks = gNumCells * gMaxNumLightsPerCell;	
				uint cellIdx = (dispatchThreadId.z * gGridSize.y + dispatchThreadId.y) * gGridSize.x + dispatchThreadId.x;
				
				// First count total number of lights affecting the tile
				uint currentIdx = gLinkedListHeads[cellIdx];
				uint numLights = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					numLights++;
					currentIdx = gLinkedList[currentIdx].y;
				}
				
				// Allocate enough room and remember the offset to indices
				uint indicesStart;
				InterlockedAdd(gGridDataCounter[0], numLights, indicesStart);
				gGridLightOffsetAndSize[cellIdx] = uint2(indicesStart, numLights);
				
				// Actually write light indices
				// Note: Values are written in the reverse order than they were found in
				currentIdx = gLinkedListHeads[cellIdx];
				uint lightIdx = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					uint2 entry = gLinkedList[currentIdx];
				
					gGridLightIndices[indicesStart + lightIdx] = entry.x;
					
					currentIdx = entry.y;
					lightIdx++;
				}
			}
		};
	};
};

Technique
{
	Language = "GLSL";
	
	Pass =
	{
		Compute = 
		{
			layout (local_size_x = THREADGROUP_SIZE, local_size_y = THREADGROUP_SIZE, local_size_z = THREADGROUP_SIZE) in;
		
			layout(binding = 0, std140) uniform Params
			{
				// Offsets at which specific light types begin in gLights buffer
				// Assumed directional lights start at 0
				// x - offset to point lights, y - offset to spot lights, z - total number of lights
				uvec3 gLightOffsets;			
				uint gNumCells;
				uvec3 gGridSize;
				uint gMaxNumLightsPerCell;
			};
			
			layout(binding = 1) uniform usamplerBuffer gLinkedListHeads;
			layout(binding = 2) uniform usamplerBuffer gLinkedList;
			
			layout(binding = 3, r32ui) uniform uimageBuffer gGridDataCounter;
			layout(binding = 4, rg32ui) uniform uimageBuffer gGridLightOffsetAndSize;
			layout(binding = 5, r32ui) uniform uimageBuffer gGridLightIndices;
			
			void main()
			{
				uvec2 viewportMax = gViewportRectangle.xy + gViewportRectangle.zw;

				// Ignore pixels out of valid range
				if (all(greaterThanEqual(gl_GlobalInvocationID.xy, viewportMax)))
					return;
					
				uint maxNumLinks = gNumCells * gMaxNumLightsPerCell;	
				uint cellIdx = (gl_GlobalInvocationID.z * gGridSize.y + gl_GlobalInvocationID.y) * gGridSize.x + gl_GlobalInvocationID.x;
				
				// First count total number of lights affecting the tile
				uint currentIdx = texelFetch(gLinkedListHeads, cellIdx).x;
				uint numLights = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					numLights++;
					currentIdx = texelFetch(gLinkedList, currentIdx).y;
				}
				
				// Allocate enough room and remember the offset to indices
				uint indicesStart = imageAtomicAdd(gGridDataCounter, 0, numLights);
				imageStore(gGridLightOffsetAndSize, cellIdx, uvec4(indicesStart, numLights, 0, 0));

				// Actually write light indices
				// Note: Values are written in the reverse order than they were found in
				currentIdx = texelFetch(gLinkedListHeads, cellIdx).x;
				uint lightIdx = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					uvec2 entry = texelFetch(gLinkedList, currentIdx).xy;
				
					imageStore(gGridLightIndices, indicesStart + lightIdx, uvec4(entry.x, 0, 0, 0));
					
					currentIdx = entry.y;
					lightIdx++;
				}
			}
		};
	};
};