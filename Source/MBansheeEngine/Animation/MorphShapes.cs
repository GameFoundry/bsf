//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @addtogroup Animation
     *  @{
     */

    /// <summary>
    /// Contains a set of morph shapes, used for morph target animation. Each morph shape contains a single possible shape
    /// that can be added on top of the base shape in order to create the animation.
    /// </summary>
    public class MorphShapes : ScriptObject
    {
        /// <summary>
        /// Constructor for internal runtime use only.
        /// </summary>
        private MorphShapes()
        { }

        /// <summary>
        /// Returns a list of names of all available morph shapes.
        /// </summary>
        public string[] Shapes
        {
            get { return Internal_GetShapes(mCachedPtr); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_GetShapes(IntPtr instance);
    }

    /** @} */
}
