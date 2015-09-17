using BansheeEngine;

namespace BansheeEditor
{
    public class HierarchyWindow : EditorWindow
    {
        private GUISceneTreeView treeView;

        [MenuItem("Windows/Hierarchy", ButtonModifier.CtrlAlt, ButtonCode.H, 6000)]
        private static void OpenHierarchyWindow()
        {
            OpenWindow<HierarchyWindow>();
        }

        protected override LocString GetDisplayName()
        {
            return new LocEdString("Hierarchy");
        }

        private void OnInitialize()
        {
            GUIScrollArea scrollArea = new GUIScrollArea();
            GUI.AddElement(scrollArea);

            treeView = new GUISceneTreeView();
            scrollArea.Layout.AddElement(treeView);
        }

        private void OnEditorUpdate()
        {
            treeView.Update();
        }
    }
}
