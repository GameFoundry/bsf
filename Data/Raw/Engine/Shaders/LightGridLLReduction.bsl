#include "$ENGINE$\PerCameraData.bslinc"
#include "$ENGINE$\LightGridCommon.bslinc"

Technique 
 : inherits("PerCameraData")
 : inherits("LightGridCommon") = 
{
	Language = "HLSL11";
	
	Pass =
	{
		Compute = 
		{
			Buffer<uint> gLightsLLHeads;
			Buffer<uint4> gLightsLL;
						
			Buffer<uint> gProbesLLHeads;
			Buffer<uint2> gProbesLL;
			
			RWBuffer<uint> gGridDataCounter;
			
			RWBuffer<uint4> gGridLightOffsetAndSize;
			RWBuffer<uint> gGridLightIndices;

			RWBuffer<uint2> gGridProbeOffsetAndSize;
			RWBuffer<uint> gGridProbeIndices;
			
			[numthreads(THREADGROUP_SIZE, THREADGROUP_SIZE, THREADGROUP_SIZE)]
			void main(
				uint3 groupId : SV_GroupID,
				uint3 groupThreadId : SV_GroupThreadID,
				uint3 dispatchThreadId : SV_DispatchThreadID)
			{
				// Ignore pixels out of valid range
				if (any(dispatchThreadId.xy >= gGridSize.xy))
					return;
					
				uint maxNumLinks = gNumCells * gMaxNumLightsPerCell;	
				uint cellIdx = (dispatchThreadId.z * gGridSize.y + dispatchThreadId.y) * gGridSize.x + dispatchThreadId.x;
				
				// Reduce lights
				//// First count total number of lights affecting the tile
				uint currentIdx = gLightsLLHeads[cellIdx];
				uint numRadialLights = 0;
				uint numSpotLights = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					uint4 entry = gLightsLL[currentIdx];
				
					if(entry.y == 1) // Radial
						numRadialLights++;
					else // Spot
						numSpotLights++;

					currentIdx = entry.z;
				}
				
				//// Allocate enough room and remember the offset to indices
				uint numLights = numRadialLights + numSpotLights;
				uint indicesStart;
				InterlockedAdd(gGridDataCounter[0], numLights, indicesStart);
				gGridLightOffsetAndSize[cellIdx] = uint4(indicesStart, numRadialLights, numSpotLights, 0);
				
				//// Actually write light indices (reverse order, so that radial lights come first, as is the convention)
				currentIdx = gLightsLLHeads[cellIdx];
				uint lightIdx = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					uint4 entry = gLightsLL[currentIdx];
				
					gGridLightIndices[indicesStart + numLights - 1 - lightIdx] = entry.x;
					
					currentIdx = entry.z;
					lightIdx++;
				}
				
				// Reduce probes
				//// First count total number of probes affecting the tile
				currentIdx = gProbesLLHeads[cellIdx];
				uint numProbes = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					uint2 entry = gProbesLL[currentIdx];
				
					numProbes++;
					currentIdx = entry.y;
				}
				
				//// Allocate enough room and remember the offset to indices
				InterlockedAdd(gGridDataCounter[1], numProbes, indicesStart);
				gGridProbeOffsetAndSize[cellIdx] = uint2(indicesStart, numProbes);
				
				//// Actually write probe indices (reverse order, in order to restore original order since LL was formed in reverse)
				currentIdx = gProbesLLHeads[cellIdx];
				uint probeIdx = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					uint2 entry = gProbesLL[currentIdx];
				
					gGridProbeIndices[indicesStart + numProbes - 1 - probeIdx] = entry.x;
					
					currentIdx = entry.y;
					probeIdx++;
				}
			}
		};
	};
};

Technique 
 : inherits("PerCameraData")
 : inherits("LightGridCommon") = 
{
	Language = "GLSL";
	
	Pass =
	{
		Compute = 
		{
			layout (local_size_x = THREADGROUP_SIZE, local_size_y = THREADGROUP_SIZE, local_size_z = THREADGROUP_SIZE) in;
		
			layout(binding = 1) uniform usamplerBuffer gLinkedListHeads;
			layout(binding = 2) uniform usamplerBuffer gLinkedList;
			
			layout(binding = 3, r32ui) uniform uimageBuffer gGridDataCounter;
			layout(binding = 5, rgba32ui) uniform uimageBuffer gGridLightOffsetAndSize;
			layout(binding = 6, r32ui) uniform uimageBuffer gGridLightIndices;
			
			void main()
			{
				// Ignore pixels out of valid range
				if (any(greaterThanEqual(gl_GlobalInvocationID.xy, gGridSize.xy)))
					return;
					
				uint maxNumLinks = gNumCells * gMaxNumLightsPerCell;	
				int cellIdx = int((gl_GlobalInvocationID.z * gGridSize.y + gl_GlobalInvocationID.y) * gGridSize.x + gl_GlobalInvocationID.x);
				
				// First count total number of lights affecting the tile
				int currentIdx = int(texelFetch(gLinkedListHeads, cellIdx).x);
				uint numRadialLights = 0;
				uint numSpotLights = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					uvec3 entry = texelFetch(gLinkedList, currentIdx).xyz;
				
					if(entry.y == 1) // Radial
						numRadialLights++;
					else // Spot
						numSpotLights++;

					currentIdx = int(entry.z);
				}
				
				// Allocate enough room and remember the offset to indices
				uint numLights = numRadialLights + numSpotLights;
				uint indicesStart = imageAtomicAdd(gGridDataCounter, 0, numLights);
				imageStore(gGridLightOffsetAndSize, cellIdx, uvec4(indicesStart, numRadialLights, numSpotLights, 0));

				// Actually write light indices (reverse order, so that radial lights come first, as is the convention)
				currentIdx = int(texelFetch(gLinkedListHeads, cellIdx).x);
				uint lightIdx = 0;
				while(currentIdx != 0xFFFFFFFF)
				{
					uvec3 entry = texelFetch(gLinkedList, currentIdx).xyz;
				
					imageStore(gGridLightIndices, int(indicesStart + numLights - 1 - lightIdx), uvec4(entry.x, 0, 0, 0));
					
					currentIdx = int(entry.z);
					lightIdx++;
				}
			}
		};
	};
};