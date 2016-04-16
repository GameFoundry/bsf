//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @addtogroup GUI-Engine
     *  @{
     */

    /// <summary>
    /// Object that allows you to group multiple GUI toggle buttons. Only one button among the grouped ones can be active.
    /// </summary>
    public sealed class GUIToggleGroup : ScriptObject
    {
        /// <summary>
        /// Creates a new toggle group.
        /// </summary>
        /// <param name="allowAllOff">If true all of the toggle buttons can be turned off, if false one will always be
        ///                           turned on.</param>
        public GUIToggleGroup(bool allowAllOff = false)
        {
            Internal_CreateInstance(this, allowAllOff);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIToggleGroup instance, bool allowAllOff);
    }

    /** @} */
}
