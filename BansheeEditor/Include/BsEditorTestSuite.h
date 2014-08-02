#pragma once

#include "BsEditorPrerequisites.h"
#include "BsTestSuite.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT EditorTestSuite : public TestSuite
	{
	public:
		EditorTestSuite();

	private:
		void SceneObjectRecord_UndoRedo();
	};
}