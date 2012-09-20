#pragma once

namespace CamelotEngine {
	// Pre-declare classes
	// Allows use of pointers in header files without including individual .h
	// so decreases dependencies between files
	class Angle;
	class AxisAlignedBox;
	class Degree;
	class Math;
	class Matrix3;
	class Matrix4;
	class Plane;
	class Quaternion;
	class Radian;
	class Ray;
	class Sphere;
	class Vector2;
	class Vector3;
	class Vector4;
	class Color;
	class DynLib;
	class DynLibManager;
	class DataStream;
	class MemoryDataStream;
	class FileDataStream;

	typedef std::shared_ptr<MemoryDataStream> MemoryDataStreamPtr;
}