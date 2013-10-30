#pragma once

namespace CamelotFramework {
	// Pre-declare classes
	// Allows use of pointers in header files without including individual .h
	// so decreases dependencies between files
	class Angle;
	class AABox;
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
	struct Int2;
	class Rect;
	class FRect;
	class Color;
	class DynLib;
	class DynLibManager;
	class DataStream;
	class MemoryDataStream;
	class FileDataStream;
	class MeshData;
	class FileSystem;
	class Timer;
	class GpuResourceData;
	class PixelData;
	class HString;
	class StringTable;
	struct LocalizedStringData;
	// Reflection
	class IReflectable;
	class RTTITypeBase;
	// Serialization
	class ISerializable;
	class SerializableType;

	typedef std::shared_ptr<DataStream> DataStreamPtr;
	typedef std::shared_ptr<MemoryDataStream> MemoryDataStreamPtr;
	typedef std::shared_ptr<FileDataStream> FileDataStreamPtr;
	typedef std::shared_ptr<MeshData> MeshDataPtr;
	typedef std::shared_ptr<PixelData> PixelDataPtr;
	typedef std::shared_ptr<GpuResourceData> GpuResourceDataPtr;
}