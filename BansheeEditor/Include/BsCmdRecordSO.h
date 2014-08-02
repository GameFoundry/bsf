#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEngine
{
	class CmdRecordSO : public EditorCommand
	{
		struct SceneObjProxy
		{
			GameObjectInstanceDataPtr instanceData;

			Vector<GameObjectInstanceDataPtr> componentInstanceData;
			Vector<SceneObjProxy> children;
		};

	public:
		~CmdRecordSO();

		static void execute(const HSceneObject& sceneObject);

		void commit();
		void revert();

	private:
		friend class UndoRedo;

		CmdRecordSO(const HSceneObject& sceneObject);
		void recordSO(const HSceneObject& sceneObject);
		void clear();
		SceneObjProxy createProxy(const HSceneObject& sceneObject);
		void restoreIds(const HSceneObject& restored);

		HSceneObject mSceneObject;
		SceneObjProxy mSceneObjectProxy;

		UINT8* mSerializedObject;
		UINT32 mSerializedObjectSize;
		UINT64 mSerializedObjectParentId;
	};
}