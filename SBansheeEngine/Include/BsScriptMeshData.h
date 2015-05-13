#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsDefaultMeshData.h"

namespace BansheeEngine
{
	// Note: Do not modify, it must match the layout of C# enum IndexType
	enum class ScriptIndexType
	{
		Index16,
		Index32
	};

	class BS_SCR_BE_EXPORT ScriptMeshData : public ScriptObject <ScriptMeshData>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "MeshData")

		DefaultMeshDataPtr getInternalValue() const { return mMeshData; }

		static MonoObject* create(const DefaultMeshDataPtr& meshData);
		static MonoObject* create(const MeshDataPtr& meshData);
	private:
		ScriptMeshData(MonoObject* managedInstance);
		~ScriptMeshData();

		void initialize(const DefaultMeshDataPtr& meshData);

		static void internal_CreateInstance(MonoObject* instance, int numVertices,
			int numIndices, VertexLayout vertex, ScriptIndexType index);
		static MonoArray* internal_GetPositions(ScriptMeshData* thisPtr);
		static void internal_SetPositions(ScriptMeshData* thisPtr, MonoArray* value);
		static MonoArray* internal_GetNormals(ScriptMeshData* thisPtr);
		static void internal_SetNormals(ScriptMeshData* thisPtr, MonoArray* value);
		static MonoArray* internal_GetTangents(ScriptMeshData* thisPtr);
		static void internal_SetTangents(ScriptMeshData* thisPtr, MonoArray* value);
		static MonoArray* internal_GetColors(ScriptMeshData* thisPtr);
		static void internal_SetColors(ScriptMeshData* thisPtr, MonoArray* value);
		static MonoArray* internal_GetUV0(ScriptMeshData* thisPtr);
		static void internal_SetUV0(ScriptMeshData* thisPtr, MonoArray* value);
		static MonoArray* internal_GetUV1(ScriptMeshData* thisPtr);
		static void internal_SetUV1(ScriptMeshData* thisPtr, MonoArray* value);
		static MonoArray* internal_GetBoneWeights(ScriptMeshData* thisPtr);
		static void internal_SetBoneWeights(ScriptMeshData* thisPtr, MonoArray* value);
		static MonoArray* internal_GetIndices(ScriptMeshData* thisPtr);
		static void internal_SetIndices(ScriptMeshData* thisPtr, MonoArray* value);
		static int internal_GetVertexCount(ScriptMeshData* thisPtr);
		static int internal_GetIndexCount(ScriptMeshData* thisPtr);

		static bool checkIsLocked(ScriptMeshData* thisPtr);
		
		DefaultMeshDataPtr mMeshData;
	};
}