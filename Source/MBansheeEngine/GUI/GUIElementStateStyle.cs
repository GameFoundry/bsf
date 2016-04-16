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
    /// Container for texture and text color used in a particular GUI element style.
    /// </summary>
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
