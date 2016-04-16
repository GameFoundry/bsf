//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Handles
     *  @{
     */

    /// <summary>
    /// Handle slider that returns a delta value as you drag the pointer along a line. For intersection purposes the line 
    /// is internally by a capsule and a sphere at its cap (assuming this will be used for arrow-like handles).
    /// </summary>
    public sealed class HandleSliderLine : HandleSlider
    {
        /// <summary>
        /// Creates a new line handle slider. 
        /// </summary>
        /// <param name="parentHandle">Handle that the slider belongs to.</param>
        /// <param name="direction">Normalized direction towards which the line is pointing.</param>
        /// <param name="length">Length of the line.</param>
        /// <param name="fixedScale">If true the handle slider will always try to maintain the same visible area in the
        ///                          viewport regardless of distance from camera.</param>
        /// <param name="layer">Layer that allows filtering of which sliders are interacted with from a specific camera.</param>
        public HandleSliderLine(Handle parentHandle, Vector3 direction, float length, bool fixedScale = true, UInt64 layer = 1)
            :base(parentHandle)
        {
            Internal_CreateInstance(this, ref direction, length, fixedScale, layer);
        }

        /// <summary>
        /// Returns a delta value that is the result of dragging/sliding the pointer along the line. This changes every 
        /// frame and will be zero unless the slider is active.
        /// </summary>
        public float Delta
        {
            get
            {
                float value;
                Internal_GetDelta(mCachedPtr, out value);
                return value;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HandleSliderLine instance, ref Vector3 direction, float length, 
            bool fixedScale, UInt64 layer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDelta(IntPtr nativeInstance, out float value);
    }

    /** @} */
}
