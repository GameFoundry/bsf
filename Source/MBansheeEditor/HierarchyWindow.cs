//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Editor window that displays the scene hierarchy tree view, displaying all scene objects in the current scene.
    /// </summary>
    public class HierarchyWindow : EditorWindow, IGlobalShortcuts
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

        /// <inheritdoc/>
        void IGlobalShortcuts.OnDeletePressed()
        {
            treeView.DeleteSelection();
        }

        /// <inheritdoc/>
        void IGlobalShortcuts.OnRenamePressed()
        {
            treeView.RenameSelection();
        }

        /// <inheritdoc/>
        void IGlobalShortcuts.OnDuplicatePressed()
        {
            treeView.DuplicateSelection();
        }

        /// <inheritdoc/>
        void IGlobalShortcuts.OnCopyPressed()
        {
            treeView.CopySelection();
        }

        /// <inheritdoc/>
        void IGlobalShortcuts.OnCutPressed()
        {
            treeView.CutSelection();
        }

        /// <inheritdoc/>
        void IGlobalShortcuts.OnPastePressed()
        {
            treeView.PasteToSelection();
        }

        private void OnInitialize()
        {
            GUIScrollArea scrollArea = new GUIScrollArea();
            GUI.AddElement(scrollArea);

            treeView = new GUISceneTreeView(GUIOption.FlexibleHeight(20), GUIOption.FlexibleWidth(20));
            scrollArea.Layout.AddElement(treeView);
        }

        private void OnEditorUpdate()
        {
            treeView.Update();
        }
    }
}
