using System;
using BansheeEngine;

namespace BansheeEditor
{
    [AttributeUsage(AttributeTargets.Method)]
    public sealed class MenuItem : Attribute
    {
        public MenuItem(string path, ButtonModifier shortcutModifier, ButtonCode shortcutKey, int priority = 0, bool separator = false)
        {
            this.path = path;
            this.shortcut = new ShortcutKey(shortcutModifier, shortcutKey);
            this.priority = priority;
            this.separator = separator;
        }

        public MenuItem(string path, int priority = 0, bool separator = false)
        {
            this.path = path;
            this.priority = priority;
            this.separator = separator;
        }

        private string path;
        private ShortcutKey shortcut;
        private int priority;
        private bool separator;
    }
}
