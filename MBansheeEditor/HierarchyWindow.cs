using BansheeEngine;

namespace BansheeEditor
{
    public class HierarchyWindow : EditorWindow
    {
        [MenuItem("Windows/Hierarchy", ButtonModifier.CtrlAlt, ButtonCode.H)]
        private static void OpenHierarchyWindow()
        {
            OpenWindow<HierarchyWindow>();
        }

        private void OnInitialize()
        {
            GUIScrollArea scrollArea = new GUIScrollArea();
            GUI.AddElement(scrollArea);

            GUISceneTreeView treeView = new GUISceneTreeView();
            scrollArea.Layout.AddElement(treeView);
        }
    }
}
