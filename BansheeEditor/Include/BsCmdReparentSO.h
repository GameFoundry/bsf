#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEngine
{
	class CmdReparentSO : public EditorCommand
	{
	public:
		static void execute(const Vector<HSceneObject>& sceneObjects, const HSceneObject& newParent);

		void commit();
		void revert();

	private:
		friend class UndoRedo;

		CmdReparentSO(const Vector<HSceneObject>& sceneObjects, const HSceneObject& newParent);

		Vector<HSceneObject> mSceneObjects;
		Vector<HSceneObject> mOldParents;
		HSceneObject mNewParent;
	};
}