//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Contains methods for opening various OS-specific browse, open and save file/folder dialogs.
    /// </summary>
    public static class BrowseDialog
    {
        /// <summary>
        /// Displays a platform specific dialog that allows the user to select file(s).
        /// </summary>
        /// <param name="defaultPath">Default path the dialog should point to.</param>
        /// <param name="filterList">Semi-colon separated list of file names or types to display in the dialog, 
        ///                          for example "exe;txt;png".</param>
        /// <param name="allowMultiselect">True if user is allowed to select multiple files.</param>
        /// <param name="outPaths">A list of file paths selected by the user.</param>
        /// <returns>True if the user selected the file(s), false if the user canceled out of the dialog.</returns>
        public static bool OpenFile(string defaultPath, string filterList, bool allowMultiselect, out string[] outPaths)
        {
            return Internal_OpenFile(defaultPath, filterList, allowMultiselect, out outPaths);
        }

        /// <summary>
        /// Displays a platform specific dialog that allows the user to select a folder.
        /// </summary>
        /// <param name="defaultPath">Default path the dialog should point to.</param>
        /// <param name="outPath">Path to the selected folder.</param>
        /// <returns>True if the user selected the folder, false if the user canceled out of the dialog.</returns>
        public static bool OpenFolder(string defaultPath, out string outPath)
        {
            return Internal_OpenFolder(defaultPath, out outPath);
        }

        /// <summary>
        /// Displays a platform specific dialog that allows the user to select where to save a file.
        /// </summary>
        /// <param name="defaultPath">Default path the dialog should point to.</param>
        /// <param name="filterList">Semi-colon separated list of file names or types to display in the dialog,
        ///                          for example "exe;txt;png".</param>
        /// <param name="outPath">File path selected by the user.</param>
        /// <returns>True if the user selected the file, false if the user canceled out of the dialog.</returns>
        public static bool SaveFile(string defaultPath, string filterList, out string outPath)
        {
            return Internal_SaveFile(defaultPath, filterList, out outPath);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_OpenFile(string defaultPath, string filterList, bool allowMultiselect, out string[] outPaths);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_OpenFolder(string defaultPath, out string outPath);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_SaveFile(string defaultPath, string filterList, out string outPath);
    }
}
