using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Object that allows you to group multiple GUI toggle buttons. Only one button among the grouped ones can be active.
    /// </summary>
    public sealed class GUIToggleGroup : ScriptObject
    {
        /// <summary>
        /// Creates a new toggle group.
        /// </summary>
        public GUIToggleGroup()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIToggleGroup instance);
    }
}
