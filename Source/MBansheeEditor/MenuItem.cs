//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Adds an entry to the main menu bar. Clicking on that entry will trigger the method the attribute is attached to.
    /// The method must be static, have no parameters or return values otherwise the attribute will be ignored.
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    public sealed class MenuItem : Attribute
    {
        /// <summary>
        /// Creates a new menu item attribute with a shortcut. Pressing the shortcut key or selecting the item in the menu
        /// will trigger the menu item callback.
        /// </summary>
        /// <param name="path">Path that determines where in the menu to add the element. All path elements must be 
        ///                    separated by /, for example "View/Toolbars/Find".</param>
        /// <param name="shortcutModifier">Optional shortcut modifier that needs to be pressed along with the shortcut 
        ///                                button in order to trigger the shortcut.</param>
        /// <param name="shortcutKey">Shortcut key that will trigger the shortcut.</param>
        /// <param name="priority">Priority determines the position of the menu item relative to its siblings.
        ///                        Higher priority means it will be placed earlier in the menu.</param>
        /// <param name="separator">Determines should a separator be inserted just before this element.</param>
        /// <param name="contextCallback">Name of a method that returns a boolean determining whether the menu item callback
        ///                               is allowed to trigger. This is useful if menu items are only valid in specific
        ///                               situations (for example a scene object must be selected). The method must be
        ///                               static, return a boolean value, accept no parameters and be in the same class as
        ///                               the method this attribute is attached to.</param>
        public MenuItem(string path, ButtonModifier shortcutModifier, ButtonCode shortcutKey, int priority = 0, 
            bool separator = false, string contextCallback = null)
        {
            this.path = path;
            this.shortcut = new ShortcutKey(shortcutModifier, shortcutKey);
            this.priority = priority;
            this.separator = separator;
            this.contextCallback = contextCallback;
        }

        /// <summary>
        /// Creates a new menu item attribute. Selecting the item in the menu will trigger the menu item callback.
        /// </summary>
        /// <param name="path">Path that determines where in the menu to add the element. All path elements must be 
        ///                    separated by /, for example "View/Toolbars/Find".</param>
        /// <param name="priority">Priority determines the position of the menu item relative to its siblings.
        ///                        Higher priority means it will be placed earlier in the menu.</param>
        /// <param name="separator">Determines should a separator be inserted just before this element.</param>
        /// <param name="contextCallback">Name of a method that returns a boolean determining whether the menu item callback
        ///                               is allowed to trigger. This is useful if menu items are only valid in specific
        ///                               situations (for example a scene object must be selected). The method must be
        ///                               static, return a boolean value, accept no parameters and be in the same class as
        ///                               the method this attribute is attached to.</param>
        public MenuItem(string path, int priority = 0, bool separator = false, string contextCallback = null)
        {
            this.path = path;
            this.priority = priority;
            this.separator = separator;
            this.contextCallback = contextCallback;
        }

        private string path;
        private ShortcutKey shortcut;
        private int priority;
        private bool separator;
        private string contextCallback;
    }
}
