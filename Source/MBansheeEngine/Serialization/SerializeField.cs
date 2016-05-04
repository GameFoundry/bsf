//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Attribute that may be used on fields to mark them as serializable. Normally public fields are serialized
    /// by default, and you can use this attribute to mark private/protected/internal fields as serialized as well.
    /// Fields must be in a serializable object otherwise this attribute has no effect.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field)]
    public sealed class SerializeField : Attribute
    {
    }

    /** @} */
}
