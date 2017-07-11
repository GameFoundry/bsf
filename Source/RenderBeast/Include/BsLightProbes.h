//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsTriangulation.h"
#include "BsMatrix4.h"
#include "BsMatrixNxM.h"

namespace bs { namespace ct
{
	class LightProbeVolume;
	struct VisibleLightProbeData;

	/** @addtogroup RenderBeast
	 *  @{
	 */

	/** Handles any pre-processing for light (irradiance) probe lighting. */
	class LightProbes
	{
		/** Internal information about a single light probe volume. */
		struct VolumeInfo
		{
			/** Volume containing the information about the probes. */
			SPtr<LightProbeVolume> volume;
			/** Remains true as long as there are dirty probes in the volume. */
			bool isDirty;
			/** Keeps track of which dirty probe was last updated, so we can perform the update over multiple frames. */
			UINT32 lastUpdatedProbe; 
		};

		/** 
		 * Information about a single tetrahedron, including neighbor information. Neighbor 4th index will be set to -1
		 * if the tetrahedron represents an outer face (which is not actually a tetrahedron, but a triangle, but is stored
		 * in the same array for convenience).
		 */
		struct TetrahedronData
		{
			Tetrahedron volume;
			Matrix4 transform;
		};
	public:
		LightProbes();

		/** Notifies sthe manager that the provided light probe volume has been added. */
		void notifyAdded(const SPtr<LightProbeVolume>& volume);

		/** Notifies the manager that the provided light probe volume has some dirty light probes. */
		void notifyDirty(const SPtr<LightProbeVolume>& volume);

		/** Notifies the manager that all the probes in the provided volume have been removed. */
		void notifyRemoved(const SPtr<LightProbeVolume>& volume);

		/**
		 * Updates any dirty light probes by rendering the scene from their perspective and generating their SH 
		 * coefficients.
		 *
		 * @param[in]	maxProbes		Places a limit of how many probes can be updated in a single call to this method.
		 *								Any probes that weren't updated will be updated when the method is called next 
		 *								(up to the @p maxProbes limit), as so on.
		 *								 
		 *								This limit is provided to ensure there are no massive framerate spikes caused up
		 *								updating many probes in a single frame - instead this method allows the updates to
		 *								be distributed over multiple frames. 
		 *								
		 *								Provide a limit of 0 to force all probes to be updated.
		 */
		void updateProbes(UINT32 maxProbes = 3);

		/** Generates GPU buffers that contain a list of probe tetrahedrons visible from the provided view. */
		void updateVisibleProbes(const RendererView& view, VisibleLightProbeData& output);

	private:
		/**
		 * Perform tetrahedrization of the provided point list, and outputs a list of tetrahedrons and outer faces of the
		 * volume. Each entry contains connections to nearby tetrahedrons/faces, as well as a matrix that can be used for
		 * calculating barycentric coordinates within the tetrahedron (or projected triangle barycentric coordinates for
		 * faces). 
		 */
		void generateTetrahedronData(const Vector<Vector3>& positions, Vector<TetrahedronData>& output, 
			bool includeOuterFaces = false);

		/** Resizes the GPU buffers used for holding tetrahedron data, to the specified size (in number of tetraheda). */
		void resizeTetrahedronBuffers(VisibleLightProbeData& data, UINT32 count);

		/** 
		 * Resized the GPU buffer that stores light probe SH coefficients, to the specified size (in the number of probes). 
		 */
		void resizeCoefficientBuffer(UINT32 count);

		Vector<VolumeInfo> mVolumes;
		bool mTetrahedronVolumeDirty;

		UINT32 mNumAllocatedEntries;
		UINT32 mNumUsedEntries;
		Vector<UINT32> mEmptyEntries;

		Vector<AABox> mTetrahedronBounds;
		Vector<TetrahedronData> mTetrahedronInfos;

		SPtr<GpuBuffer> mProbeCoefficientsGPU;

		// Temporary buffers
		Vector<Vector3> mTempTetrahedronPositions;
		Vector<UINT32> mTempTetrahedronVisibility;
	};

	/** Storage of tetrahedron AA box, for use on the GPU. */
	struct TetrahedronBoundsGPU
	{
		Vector4 center;
		Vector4 extents;
	};

	/** Information about a single tetrahedron, for use on the GPU. */
	struct TetrahedronDataGPU
	{
		UINT32 indices[4];
		Matrix3x4 transform;
	};

	/** Contains information about light probes visible from a particular RendererView. */
	struct VisibleLightProbeData
	{
		/** Current number of visible tetrahedrons in the GPU buffers. */
		UINT32 numEntries;

		/** Maximum number of tetrahedrons that fit in the GPU buffers, before the buffers need to be resized. */
		UINT32 maxNumEntries;
		
		/** GPU buffer containing tetrahedron bounds in form of TetrahedronBoundsGPU structure. */
		SPtr<GpuBuffer> tetrahedronBounds;

		/** GPU buffer containing tetrahedron information in form of TetrahedronDataGPU structure. */
		SPtr<GpuBuffer> tetrahedronInfos;
	};

	/** @} */
}}
