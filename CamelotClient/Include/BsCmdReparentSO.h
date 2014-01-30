#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorCommand.h"
#include "BsUndoRedo.h"

namespace BansheeEditor
{
	class CmdReparentSO : public EditorCommand
	{
	public:
		static void execute(const CM::Vector<CM::HSceneObject>::type& sceneObjects, const CM::HSceneObject& newParent);

		void commit();
		void revert();

	private:
		friend class UndoRedo;

		CmdReparentSO(const CM::Vector<CM::HSceneObject>::type& sceneObjects, const CM::HSceneObject& newParent);

		CM::Vector<CM::HSceneObject>::type mSceneObjects;
		CM::Vector<CM::HSceneObject>::type mOldParents;
		CM::HSceneObject mNewParent;
	};
}