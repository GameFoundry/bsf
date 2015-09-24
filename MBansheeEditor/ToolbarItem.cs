using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Adds an entry to the main tool. Clicking on that entry will trigger the method the attribute is attached to.
    /// The method must be static, have no parameters or return values otherwise the attribute will be ignored.
    /// </summary>
    [AttributeUsage(AttributeTargets.Method)]
    class ToolbarItem : Attribute
    {
        /// <summary>
        /// Creates a new toolbar item attribute. Selecting the item in the toolbar will trigger the callback.
        /// </summary>
        /// <param name="name">Path that determines where in the menu to add the element. All path elements must be 
        ///                    separated by /, e.g. "View/Toolbars/Find"</param>
        /// <param name="icon">Path to the sprite texture to display on the toolbar entry. Relative to project library 
        ///                    resources folder.</param>
        /// <param name="tooltip">Optional tooltip to display when the user hovers over the toolbar entry.</param>
        /// <param name="priority">Priority determines the position of the toolbar item relative to its siblings.
        ///                        Higher priority means it will be placed earlier.</param>
        /// <param name="separator">Determines should a separator be inserted just before this element.</param>
        public ToolbarItem(string name, string icon, string tooltip = "", int priority = 0, bool separator = false)
        {
            this.name = name;
            this.icon = icon;
            this.tooltip = tooltip;
            this.priority = priority;
            this.separator = separator;
            this.builtinIcon = -1;
        }

        /// <summary>
        /// Creates a new toolbar item attribute. Selecting the item in the toolbar will trigger the callback.
        /// </summary>
        /// <param name="name">Path that determines where in the menu to add the element. All path elements must be 
        ///                    separated by /, e.g. "View/Toolbars/Find"</param>
        /// <param name="icon">Type of builtin icon to display on the toolbar entry.</param>
        /// <param name="tooltip">Optional tooltip to display when the user hovers over the toolbar entry.</param>
        /// <param name="priority">Priority determines the position of the toolbar item relative to its siblings.
        ///                        Higher priority means it will be placed earlier.</param>
        /// <param name="separator">Determines should a separator be inserted just before this element.</param>
        internal ToolbarItem(string name, ToolbarIcon icon, string tooltip = "", int priority = 0, bool separator = false)
        {
            this.name = name;
            this.icon = null;
            this.tooltip = tooltip;
            this.priority = priority;
            this.separator = separator;
            this.builtinIcon = (int)icon;
        }

        private string name;
        private string icon;
        private int builtinIcon;
        private string tooltip;
        private int priority;
        private bool separator;
    }
}
