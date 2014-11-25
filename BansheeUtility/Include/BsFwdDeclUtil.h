#pragma once

// Contains forward declares used as a convenience

namespace BansheeEngine 
{
	/**
	 * @brief	Values that represent in which order are euler
	 * 			angles applied when used in transformations
	 */
	enum class EulerAngleOrder
	{
		XYZ,
		XZY,
		YXZ,
		YZX,
		ZXY,
		ZYX
	};

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
	struct Vector2I;
	class Rect2I;
	class Rect2;
	class Rect3;
	class Color;
	class DynLib;
	class DynLibManager;
	class DataStream;
	class MemoryDataStream;
	class FileDataStream;
	class MeshData;
	class FileSystem;
	class Timer;
	class Task;
	class GpuResourceData;
	class PixelData;
	class HString;
	class StringTable;
	struct LocalizedStringData;
	class Path;
	class HThread;
	class TestSuite;
	class TestOutput;
	class AsyncOpSyncData;
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
	typedef std::shared_ptr<DataStream> DataStreamPtr;
	typedef std::shared_ptr<MemoryDataStream> MemoryDataStreamPtr;
	typedef std::shared_ptr<Task> TaskPtr;
	typedef std::shared_ptr<TestSuite> TestSuitePtr;
	typedef std::shared_ptr<AsyncOpSyncData> AsyncOpSyncDataPtr;

	typedef List<DataStreamPtr> DataStreamList;
	typedef std::shared_ptr<DataStreamList> DataStreamListPtr;

	enum TypeID_Utility
	{
		TID_Abstract = 50, // Special type ID used for Abstract classes. Only type ID that may be used by more than one class.
		TID_WString = 51,
		TID_Path = 52,
		TID_Vector = 53,
		TID_Map = 54,
		TID_UnorderedMap = 55,
		TID_Pair = 56,
		TID_Set = 57,
	};
}