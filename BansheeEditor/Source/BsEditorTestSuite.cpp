#include "BsEditorTestSuite.h"

namespace BansheeEngine
{
	EditorTestSuite::EditorTestSuite()
	{
		BS_ADD_TEST(EditorTestSuite::SceneObjectRecord_UndoRedo)
	}

	void EditorTestSuite::SceneObjectRecord_UndoRedo()
	{
		BS_TEST_ASSERT_MSG(false, "SUP SUP");
	}
}