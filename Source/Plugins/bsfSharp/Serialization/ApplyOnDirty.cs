//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2018 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    /// <summary>
    /// Attribute that signifies if a property changes those changes might not be recognized by the parent object until
    /// the property setter is called. This lets the system (such as the inspector window) know that the value needs to be
    /// re-assigned to the property if any changes to it are made.
    /// </summary>
    [AttributeUsage(AttributeTargets.Property)]
    public sealed class ApplyOnDirty : Attribute { }

    /** @} */
}
