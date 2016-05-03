//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @cond INTEROP */
    /** @addtogroup Interop
     *  @{
     */

    /// <summary>
    /// Wrapper around the native PlaneCollider class.
    /// <see cref="PlaneCollider"/>
    /// </summary>
    internal class NativePlaneCollider : NativeCollider
    {
        public NativePlaneCollider()
        {
            Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(NativePlaneCollider instance);
    }

    /** @} */
    /** @endcond */
}
