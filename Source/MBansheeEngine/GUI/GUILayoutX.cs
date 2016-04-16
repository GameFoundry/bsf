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
    /// Horizontal layout that will position its child elements left to right.
    /// </summary>
    public sealed class GUILayoutX : GUILayout
    {
        /// <summary>
        /// Constructs a new empty horizontal layout.
        /// </summary>
        /// <param name="options">Options that allow you to control how is the layout positioned and sized.</param>
        public GUILayoutX(params GUIOption[] options)
        {
            Internal_CreateInstanceX(this, options);
        }
    }

    /** @} */
}
