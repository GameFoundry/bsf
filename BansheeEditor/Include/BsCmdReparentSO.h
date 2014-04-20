#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEngine
{
	class CmdReparentSO : public EditorCommand
	{
	public:
		static void execute(const Vector<HSceneObject>::type& sceneObjects, const HSceneObject& newParent);

		void commit();
		void revert();

	private:
		friend class UndoRedo;

		CmdReparentSO(const Vector<HSceneObject>::type& sceneObjects, const HSceneObject& newParent);

		Vector<HSceneObject>::type mSceneObjects;
		Vector<HSceneObject>::type mOldParents;
		HSceneObject mNewParent;
	};
}