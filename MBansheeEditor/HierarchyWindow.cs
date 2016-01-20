//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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

        /// <summary>
        /// Cuts the currently selected scene object.
        /// </summary>
        public void CutSelection()
        {
            treeView.CutSelection();
        }

        /// <summary>
        /// Copies the currently selected scene object.
        /// </summary>
        public void CopySelection()
        {
            treeView.CopySelection();
        }

        /// <summary>
        /// Pastes the scene object(s) that were previously cut or copied.
        /// </summary>
        public void PasteToSelection()
        {
            treeView.PasteToSelection();
        }

        /// <summary>
        /// Deletes currently selected scene objects.
        /// </summary>
        public void DeleteSelection()
        {
            treeView.DeleteSelection();
        }

        /// <summary>
        /// Duplicates currently selected scene objects.
        /// </summary>
        public void DuplicateSelection()
        {
            treeView.DuplicateSelection();
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
