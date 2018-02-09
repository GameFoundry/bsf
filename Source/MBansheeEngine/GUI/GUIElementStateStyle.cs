//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup GUI_Engine
     *  @{
     */

    /// <summary>
    /// Container for texture and text color used in a particular GUI element style.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct GUIElementStateStyle
    {
        /// <summary>
        /// Texture used by the GUI element style state. Can be null.
        /// </summary>
        public SpriteTexture Texture;

        /// <summary>
        /// Color applied to the text when GUI element style state is active.
        /// </summary>
        public Color TextColor;
    };

    /** @} */
}
