//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsTriangulation.h"
#include "BsVector3.h"

// Third party
#include "TetGen/tetgen.h"

namespace bs
{
	TetrahedronVolume Triangulation::tetrahedralize(const Vector<Vector3>& points)
	{
		TetrahedronVolume volume;
		if (points.size() < 4)
			return volume;

		tetgenio input;
		input.numberofpoints = (int)points.size();
		input.pointlist = new REAL[input.numberofpoints * 3]; // Must be allocated with "new" because TetGen deallocates it using "delete"
		for(UINT32 i = 0; i < (UINT32)points.size(); ++i)
		{
			input.pointlist[i * 3 + 0] = points[i].x;
			input.pointlist[i * 3 + 1] = points[i].y;
			input.pointlist[i * 3 + 2] = points[i].z;
		}

		tetgenbehavior options;
		options.neighout = 2; // Generate adjacency information between tets and outer faces
		options.facesout = 1; // Output face adjacency
		options.quiet = 1; // Don't print anything

		tetgenio output;
		::tetrahedralize(&options, &input, &output);

		UINT32 numTetrahedra = (UINT32)output.numberoftetrahedra;
		volume.tetrahedra.resize(numTetrahedra);

		for (UINT32 i = 0; i < numTetrahedra; ++i)
		{
			memcpy(volume.tetrahedra[i].vertices, &output.tetrahedronlist[i * 4], sizeof(INT32) * 4);
			memcpy(volume.tetrahedra[i].neighbors, &output.neighborlist[i * 4], sizeof(INT32) * 4);
		}

		UINT32 numFaces = (UINT32)output.numberoftrifaces;
		for (UINT32 i = 0; i < numFaces; ++i)
		{
			INT32 tetIdx = -1;
			if (output.adjtetlist[i * 2] == -1)
				tetIdx = output.adjtetlist[i * 2 + 1];
			else if (output.adjtetlist[i * 2 + 1] == -1)
				tetIdx = output.adjtetlist[i * 2];
			else // Not a boundary face
				continue;

			volume.outerFaces.push_back(TetrahedronFace());
			TetrahedronFace& face = volume.outerFaces.back();

			memcpy(face.vertices, &output.trifacelist[i * 3], sizeof(INT32) * 3);
			face.tetrahedron = tetIdx;
		}

		return volume;
	}
}