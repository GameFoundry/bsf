using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Editor window that displays the scene hierarchy tree view, displaying all scene objects in the current scene.
    /// </summary>
    public class HierarchyWindow : EditorWindow
    {
        private GUISceneTreeView treeView;

        /// <summary>
        /// Opens the hierarchy window.
        /// </summary>
        [MenuItem("Windows/Hierarchy", ButtonModifier.CtrlAlt, ButtonCode.H, 6000)]
        private static void OpenHierarchyWindow()
        {
            OpenWindow<HierarchyWindow>();
        }

        /// <inheritdoc/>
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
