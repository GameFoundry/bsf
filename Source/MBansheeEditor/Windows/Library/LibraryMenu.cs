//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.IO;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Library
     *  @{
     */

    /// <summary>
    /// Handles main menu and context menu items and callbacks for project library window.
    /// </summary>
    internal static class LibraryMenu
    {
        /// <summary>
        /// Creates the context menu used by project library window. New context menu must be created when a new instance
        /// of the project library window is created.
        /// </summary>
        /// <param name="win">Instance of the project library window.</param>
        /// <returns>Context menu bound to the specified instance of the project library window.</returns>
        internal static ContextMenu CreateContextMenu(LibraryWindow win)
        {
            ContextMenu entryContextMenu = new ContextMenu();
            entryContextMenu.AddItem("Create", null);
            entryContextMenu.AddItem("Create/Folder", CreateFolder);
            entryContextMenu.AddItem("Create/Material", CreateEmptyMaterial);
            entryContextMenu.AddItem("Create/Physics material", CreateEmptyPhysicsMaterial);
            entryContextMenu.AddItem("Create/Shader", CreateEmptyShader);
            entryContextMenu.AddItem("Create/C# script", CreateEmptyCSScript);
            entryContextMenu.AddItem("Create/Sprite texture", CreateEmptySpriteTexture);
            entryContextMenu.AddItem("Create/GUI skin", CreateEmptyGUISkin);
            entryContextMenu.AddItem("Create/String table", CreateEmptyStringTable);
            entryContextMenu.AddSeparator("");
            entryContextMenu.AddItem("Rename", win.RenameSelection, new ShortcutKey(ButtonModifier.None, ButtonCode.F2));
            entryContextMenu.AddSeparator("");
            entryContextMenu.AddItem("Cut", win.CutSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.X));
            entryContextMenu.AddItem("Copy", win.CopySelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.C));
            entryContextMenu.AddItem("Duplicate", win.DuplicateSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.D));
            entryContextMenu.AddItem("Paste", win.PasteToSelection, new ShortcutKey(ButtonModifier.Ctrl, ButtonCode.V));
            entryContextMenu.AddSeparator("");
            entryContextMenu.AddItem("Delete", win.DeleteSelection, new ShortcutKey(ButtonModifier.None, ButtonCode.Delete));
            entryContextMenu.AddSeparator("");
            entryContextMenu.AddItem("Open externally", OpenExternally);
            entryContextMenu.AddItem("Explore location", ExploreLocation);

            entryContextMenu.SetLocalizedName("Rename", new LocEdString("Rename"));
            entryContextMenu.SetLocalizedName("Cut", new LocEdString("Cut"));
            entryContextMenu.SetLocalizedName("Copy", new LocEdString("Copy"));
            entryContextMenu.SetLocalizedName("Duplicate", new LocEdString("Duplicate"));
            entryContextMenu.SetLocalizedName("Paste", new LocEdString("Paste"));
            entryContextMenu.SetLocalizedName("Delete", new LocEdString("Delete"));

            return entryContextMenu;
        }

        /// <summary>
        /// Queries if a library window is displayed.
        /// </summary>
        /// <returns>True if a library window is active, false if not.</returns>
        internal static bool IsLibraryWindowActive()
        {
            return EditorWindow.GetWindow<LibraryWindow>() != null;
        }

        /// <summary>
        /// Creates a new material with the default shader in the currently selected project library folder.
        /// </summary>
        [MenuItem("Resources/Create/Folder", 9051, false, "IsLibraryWindowActive")]
        internal static void CreateFolder()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            LibraryUtility.CreateFolder(win.SelectedFolder);
        }

        /// <summary>
        /// Creates a new material with the default shader in the currently selected project library folder.
        /// </summary>
        [MenuItem("Resources/Create/Material", 9050, false, "IsLibraryWindowActive")]
        [ToolbarItem("Material", ToolbarIcon.NewMat, "New material", 1498)]
        internal static void CreateEmptyMaterial()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if(win == null)
                return;

            LibraryUtility.CreateEmptyMaterial(win.SelectedFolder);
        }

        /// <summary>
        /// Creates a new shader containing a rough code outline in the currently selected project library folder.
        /// </summary>
        [MenuItem("Resources/Create/Shader", 9049, false, "IsLibraryWindowActive")]
        [ToolbarItem("Shader", ToolbarIcon.NewShader, "New shader", 1499)]
        internal static void CreateEmptyShader()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            LibraryUtility.CreateEmptyShader(win.SelectedFolder);
        }

        /// <summary>
        /// Creates a new C# script containing a rough code outline in the currently selected project library folder.
        /// </summary>
        [MenuItem("Resources/Create/C# script", 9048, false, "IsLibraryWindowActive")]
        [ToolbarItem("C# script", ToolbarIcon.NewCSScript, "New C# script", 1500, true)]
        internal static void CreateEmptyCSScript()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            LibraryUtility.CreateEmptyCSScript(win.SelectedFolder);
        }

        /// <summary>
        /// Creates a new empty sprite texture in the currently selected project library folder.
        /// </summary>
        [MenuItem("Resources/Create/Sprite texture", 9047, false, "IsLibraryWindowActive")]
        [ToolbarItem("Sprite texture", ToolbarIcon.NewSpriteTex, "New sprite texture", 1497)]
        internal static void CreateEmptySpriteTexture()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            LibraryUtility.CreateEmptySpriteTexture(win.SelectedFolder);
        }

        /// <summary>
        /// Creates a new empty GUI skin in the currently selected project library folder.
        /// </summary>
        [MenuItem("Resources/Create/GUI skin", 9046, false, "IsLibraryWindowActive")]
        internal static void CreateEmptyGUISkin()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            LibraryUtility.CreateEmptyGUISkin(win.SelectedFolder);
        }

        /// <summary>
        /// Creates a new empty string table in the currently selected project library folder.
        /// </summary>
        [MenuItem("Resources/Create/String table", 9045, false, "IsLibraryWindowActive")]
        internal static void CreateEmptyStringTable()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            LibraryUtility.CreateEmptyStringTable(win.SelectedFolder);
        }

        /// <summary>
        /// Creates a new physics material with the default properties in the currently selected project library folder.
        /// </summary>
        [MenuItem("Resources/Create/Physics material", 9044, false, "IsLibraryWindowActive")]
        internal static void CreateEmptyPhysicsMaterial()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            LibraryUtility.CreateEmptyPhysicsMaterial(win.SelectedFolder);
        }

        /// <summary>
        /// Opens the currently selected project library file or folder in the default external application.
        /// </summary>
        [MenuItem("Resources/Open externally", 9040, true, "IsLibraryWindowActive")]
        internal static void OpenExternally()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            EditorApplication.OpenFolder(Path.Combine(ProjectLibrary.ResourceFolder, win.SelectedEntry));
        }

        /// <summary>
        /// Explores the current project library folder in the external file system explorer.
        /// </summary>
        [MenuItem("Resources/Explore location", 9039, false, "IsLibraryWindowActive")]
        internal static void ExploreLocation()
        {
            LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
            if (win == null)
                return;

            EditorApplication.OpenFolder(Path.Combine(ProjectLibrary.ResourceFolder, win.CurrentFolder));
        }
    }

    /** @} */
}
