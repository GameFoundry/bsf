//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Contains data used for initializing a context menu used on GUI elements. When specifying menu items you must provide
    /// a path. Path must be formated in a certain way. All path elements must be separated by /, for example 
    /// "View/Toolbars/Find". "View" would be the top level path element, "Toolbars" a child in its menu that opens up its
    /// own submenu, and "Find" a child in the "Toolbars" sub-menu with an optional callback.
    /// </summary>
    public class ContextMenu : ScriptObject
    {
        private List<Action> callbacks = new List<Action>(); 

        /// <summary>
        /// Creates a new empty context menu data.
        /// </summary>
        public ContextMenu()
        {
            Internal_CreateInstance(this);
        }

        /// <summary>
        /// Adds a new item to the menu.
        /// </summary>
        /// <param name="path">Path that determines where to add the element. See class information on how to specify paths.
        ///                    All sub-elements of a path will be added automatically.</param>
        /// <param name="callback">Callback to trigger when the path element is selected.</param>
        public void AddItem(string path, Action callback)
        {
            Internal_AddItem(mCachedPtr, path, callbacks.Count, ref ShortcutKey.None);
            callbacks.Add(callback);
        }

        /// <summary>
        /// Adds a new item to the menu.
        /// </summary>
        /// <param name="path">Path that determines where to add the element. See class information on how to specify paths.
        ///                    All sub-elements of a path will be added automatically.</param>
        /// <param name="callback">Callback to trigger when the path element is selected.</param>
        /// <param name="shortcut">Keyboard shortcut to display next to the item name.</param>
        public void AddItem(string path, Action callback, ShortcutKey shortcut)
        {
            Internal_AddItem(mCachedPtr, path, callbacks.Count, ref shortcut);
            callbacks.Add(callback);
        }

        /// <summary>
        /// Adds a new item to the menu.
        /// </summary>
        /// <param name="path">Path that determines where to add the element. See class information on how to specify paths.
        ///                    All sub-elements of a path will be added automatically.</param>
        /// <param name="callback">Callback to trigger when the path element is selected.</param>
        /// <param name="name">Localized name for the menu item.</param>
        public void AddItem(string path, Action callback, LocString name)
        {
            Internal_AddItem(mCachedPtr, path, callbacks.Count, ref ShortcutKey.None);
            callbacks.Add(callback);
        }

        /// <summary>
        /// Adds a new item to the menu.
        /// </summary>
        /// <param name="path">Path that determines where to add the element. See class information on how to specify paths.
        ///                    All sub-elements of a path will be added automatically.</param>
        /// <param name="callback">Callback to trigger when the path element is selected.</param>
        /// <param name="shortcut">Keyboard shortcut to display next to the item name.</param>
        /// <param name="name">Localized name for the menu item.</param>
        public void AddItem(string path, Action callback, ShortcutKey shortcut, LocString name)
        {
            Internal_AddItem(mCachedPtr, path, callbacks.Count, ref shortcut);
            callbacks.Add(callback);
        }

        /// <summary>
        /// Adds a new separator to the menu.
        /// </summary>
        /// <param name="path">Path that determines where to add the element. See class information on how to specify paths.
        ///                    All sub-elements of a path will be added automatically.</param>
        public void AddSeparator(string path)
        {
            Internal_AddSeparator(mCachedPtr, path);
        }

        /// <summary>
        /// Sets a localized name of a specific menu element. If no localized name is set element labels will be displayed
        /// as their names.
        /// </summary>
        /// <param name="label">Label of the element.</param>
        /// <param name="name">Name to display when the menu is shown.</param>
        public void SetLocalizedName(string label, LocString name)
        {
            IntPtr namePtr = IntPtr.Zero;
            if (name != null)
                namePtr = name.GetCachedPtr();

            Internal_SetLocalizedName(mCachedPtr, label, namePtr);
        }

        /// <summary>
        /// Triggered by the runtime when an element is selected the context menu.
        /// </summary>
        /// <param name="callbackIdx">Unique index of the element that was selected.</param>
        private void InternalDoOnEntryTriggered(int callbackIdx)
        {
            if (callbackIdx < 0 || callbackIdx >= callbacks.Count)
                return;

            Action callback = callbacks[callbackIdx];
            if (callback != null)
                callback();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ContextMenu instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_AddItem(IntPtr instance, string path, int callbackIdx, ref ShortcutKey shortcut);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_AddSeparator(IntPtr instance, string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLocalizedName(IntPtr instance, string label, IntPtr name);
    }
}
