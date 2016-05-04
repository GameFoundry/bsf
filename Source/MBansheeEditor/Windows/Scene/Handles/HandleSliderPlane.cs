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
    /// Handle slider that returns a delta value as you drag the pointer along a plane. For intersection purposes the line 
    /// is internally by a quadrilateral (a bounded plane).
    /// </summary>
    public sealed class HandleSliderPlane : HandleSlider
    {
        /// <summary>
        /// Creates a new plane handle slider. 
        /// </summary>
        /// <param name="parentHandle">Handle that the slider belongs to.</param>
        /// <param name="dir1">First direction of the plane. The x component of returned delta value will be in this 
        ///                    direction. Should be perpendicular to <see cref="dir2"/>.</param>
        /// <param name="dir2">Second direction of the plane. The y component of returned delta value will be in this
        ///                    direction. Should be perpendicular to <see cref="dir1"/>.</param>
        /// <param name="length">Length of the quadrilateral in both directions.</param>
        /// <param name="fixedScale">If true the handle slider will always try to maintain the same visible area in the
        ///                          viewport regardless of distance from camera.</param>
        /// <param name="layer">Layer that allows filtering of which sliders are interacted with from a specific camera.</param>
        public HandleSliderPlane(Handle parentHandle, Vector3 dir1, Vector3 dir2, float length, bool fixedScale = true, UInt64 layer = 1)
            :base(parentHandle)
        {
            Internal_CreateInstance(this, ref dir1, ref dir2, length, fixedScale, layer);
        }

        /// <summary>
        /// Returns a delta value that is the result of dragging/sliding the pointer along the plane. Returned movement is 
        /// in terms of the two directions originally provided when constructing the slider. This changes every frame and 
        /// will be zero unless the slider is active.
        /// </summary>
        public Vector2 Delta
        {
            get
            {
                Vector2 value;
                Internal_GetDelta(mCachedPtr, out value);
                return value;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HandleSliderPlane instance, ref Vector3 dir1, ref Vector3 dir2, 
            float length, bool fixedScale, UInt64 layer);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDelta(IntPtr nativeInstance, out Vector2 value);
    }

    /** @} */
}
