//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2018 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Attribute that signifies that a field or a property passes its value by copy. This lets the system (such as the
    /// inspector window) know that the value needs to be re-assigned to the field/property if any changes to it are made.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field | AttributeTargets.Property)]
    public sealed class PassByCopy : Attribute { }

    /** @} */
}
