//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Vertical layout that will position its child elements top to bottom.
    /// </summary>
    public sealed class GUILayoutY : GUILayout
    {
        /// <summary>
        /// Internal method used by the runtime. Initializes a managed version of the vertical layout that is referenced
        /// by a native GUI scroll area element.
        /// </summary>
        /// <param name="parentArea">Scroll area of which we want to reference the layout of.</param>
        internal GUILayoutY(GUIScrollArea parentArea)
        {
            Internal_CreateInstanceYFromScrollArea(this, parentArea);
        }

        /// <summary>
        /// Vertical layout that will position its child elements left to right.
        /// </summary>
        public GUILayoutY(params GUIOption[] options)
        {
            Internal_CreateInstanceY(this, options);
        }
    }
}
