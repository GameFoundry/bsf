//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2018 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Changes the editor inspector GUI element of a 64-bit integer to a layer-mask selection drop down. 
    /// </summary>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class LayerMask : Attribute
    {
        /// <summary>
        /// Creates a new layer-mask attribute.
        /// </summary>
        public LayerMask()
        { }
    }

    /** @} */
}
