#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptMeshData.h"
#include "BsMesh.h"

namespace BansheeEngine
{
	enum class MeshTopology
	{
		PointList = 1,
		LineList = 2,
		LineStrip = 3,
		TriangleList = 4,
		TriangleStrip = 5,
		TriangleFan = 6
	};

	struct BS_SCR_BE_EXPORT SubMeshData
	{
		UINT32 indexOffset;
		UINT32 indexCount;
		MeshTopology topology;
	};

	class BS_SCR_BE_EXPORT ScriptSubMesh : public ScriptObject < ScriptSubMesh >
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "SubMesh")

		static SubMeshData unbox(MonoObject* obj);
		static MonoObject* box(const SubMeshData& value);

	private:
		ScriptSubMesh(MonoObject* instance);
	};

	class BS_SCR_BE_EXPORT ScriptMesh : public ScriptObject <ScriptMesh, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Mesh")

		HResource getNativeHandle() const { return mMesh; }
		void setNativeHandle(const HResource& resource);

		HMesh getMeshHandle() const { return mMesh; }
	private:
		friend class ScriptResourceManager;

		ScriptMesh(MonoObject* instance, const HMesh& mesh);

		static void internal_CreateInstance(MonoObject* instance, int numVertices,
			int numIndices, MonoArray* subMeshes, MeshUsage usage, VertexLayout vertex, ScriptIndexType index);
		static void internal_CreateInstanceMeshData(MonoObject* instance, ScriptMeshData* data, MonoArray* subMeshes,
			MeshUsage usage);
		static MonoArray* internal_GetSubMeshes(ScriptMesh* thisPtr);
		static UINT32 internal_GetSubMeshCount(ScriptMesh* thisPtr);
		static void internal_GetBounds(ScriptMesh* thisPtr, AABox* box, Sphere* sphere);
		static MonoObject* internal_GetMeshData(ScriptMesh* thisPtr);
		static void internal_SetMeshData(ScriptMesh* thisPtr, ScriptMeshData* value);

		static DrawOperationType meshTopologyToDrawOp(MeshTopology topology);
		static MeshTopology drawOpToMeshTopology(DrawOperationType drawOp);
		static Vector<SubMesh> monoToNativeSubMeshes(MonoArray* subMeshes);

		void _onManagedInstanceDeleted();

		HMesh mMesh;
	};
}