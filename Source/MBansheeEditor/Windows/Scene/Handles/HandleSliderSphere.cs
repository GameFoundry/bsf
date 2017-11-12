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
    /// Handle slider represented by a sphere collider. The slider doesn't allow dragging (it doesn't report a delta value
    /// like other sliders) but can be used for picking (selection) purposes.
    /// </summary>
    public sealed class HandleSliderSphere : HandleSlider
    {
        /// <summary>
        /// Creates a new sphere handle slider. 
        /// </summary>
        /// <param name="parentHandle">Handle that the slider belongs to.</param>
        /// <param name="radius">Radius of the collider sphere.</param>
        /// <param name="fixedScale">If true the handle slider will always try to maintain the same visible area in the
        ///                          viewport regardless of distance from camera.</param>
        /// <param name="layer">Layer that allows filtering of which sliders are interacted with from a specific camera.</param>
        public HandleSliderSphere(Handle parentHandle, float radius, bool fixedScale = true, UInt64 layer = 1)
            :base(parentHandle)
        {
            Internal_CreateInstance(this, radius, fixedScale, layer);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(HandleSliderSphere instance, float radius, bool fixedScale, 
            UInt64 layer);
    }

    /** @} */
}
