//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;

namespace BansheeEngine
{
    /// <summary>
    /// Attribute that prevents serialization of a field that would otherwise be serialized. Normally those fields
    /// are public fields of a <see cref="Component"/>, <see cref="Resource"/> or a class marked with a 
    /// <see cref="SerializeObject"/> attribute.
    /// </summary>
    [AttributeUsage(AttributeTargets.Field)]
    public sealed class DontSerializeField : Attribute
    {
    }
}
