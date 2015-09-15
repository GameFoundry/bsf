using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
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
        public HandleSliderPlane(Handle parentHandle, Vector3 dir1, Vector3 dir2, float length, bool fixedScale = true)
            :base(parentHandle)
        {
            Internal_CreateInstance(this, dir1, dir2, length, fixedScale);
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
        private static extern void Internal_CreateInstance(HandleSliderPlane instance, Vector3 dir1, Vector3 dir2, float length, bool fixedScale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDelta(IntPtr nativeInstance, out Vector2 value);
    }
}
