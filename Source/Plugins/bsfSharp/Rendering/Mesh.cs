//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup Rendering
     *  @{
     */

    public partial class Mesh
    {
        /// <summary>
        /// Returns local bounds of the geometry contained in the vertex buffers for all sub-meshes.
        /// </summary>
        public Bounds Bounds
        {
            get
            {
                AABox box;
                Sphere sphere;

                Internal_getBounds(mCachedPtr, out box, out sphere);

                return new Bounds(box, sphere);
            }
        }
    }

    /** @} */
}
