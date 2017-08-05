//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsTriangulation.h"
#include "BsMatrix4.h"
#include "BsMatrixNxM.h"

namespace bs { namespace ct
{
	struct FrameInfo;
	class LightProbeVolume;

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

		/** Updates light probe tetrahedron data after probes changed (added/removed/moved). */
		void updateProbes();

	private:
		/**
		 * Perform tetrahedrization of the provided point list, and outputs a list of tetrahedrons and outer faces of the
		 * volume. Each entry contains connections to nearby tetrahedrons/faces, as well as a matrix that can be used for
		 * calculating barycentric coordinates within the tetrahedron (or projected triangle barycentric coordinates for
		 * faces). 
		 * 
		 * @param[in,out]	positions					A set of positions to generate the tetrahedra from. If 
		 *												@p generateExtrapolationVolume is enabled then this array will be
		 *												appended with new vertices forming that volume.
		 * @param[out]		output						A list of generated tetrahedra and relevant data.
		 * @param[in]		generateExtrapolationVolume	If true, the tetrahedron volume will be surrounded with points
		 *												at "infinity" (technically just far away).
		 */
		void generateTetrahedronData(Vector<Vector3>& positions, Vector<TetrahedronData>& output, 
			bool generateExtrapolationVolume = false);

		/** Resizes the GPU buffer used for holding tetrahedron data, to the specified size (in number of tetraheda). */
		void resizeTetrahedronBuffer(UINT32 count);

		/** 
		 * Resized the GPU buffer that stores light probe SH coefficients, to the specified size (in the number of probes). 
		 */
		void resizeCoefficientBuffer(UINT32 count);

		Vector<VolumeInfo> mVolumes;
		bool mTetrahedronVolumeDirty;

		UINT32 mMaxCoefficients;
		UINT32 mMaxTetrahedra;

		Vector<TetrahedronData> mTetrahedronInfos;

		SPtr<GpuBuffer> mProbeCoefficientsGPU;
		SPtr<GpuBuffer> mTetrahedronInfosGPU;
		SPtr<Mesh> mVolumeMesh;

		// Temporary buffers
		Vector<Vector3> mTempTetrahedronPositions;
		Vector<UINT32> mTempTetrahedronBufferIndices;
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

	/** @} */
}}
