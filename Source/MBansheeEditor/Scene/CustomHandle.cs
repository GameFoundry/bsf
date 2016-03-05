//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Attribute that can be added to types deriving from <see cref="BansheeEditor.Handle"/>. That handle implementation
    /// will then be used whenever a handle for the type specified in this attribute needs to be displayed.
    /// 
    /// Implementation must contain a constructor accepting the object for the type the handle is shown for, otherwise
    /// it will not be recognized by the system. If handle is not shown for any specific type, the constructor should not
    /// accept any parameters.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class CustomHandle : Attribute
    {
        private Type type;

        /// <summary>
        /// Creates a new custom handle attribute.
        /// </summary>
        /// <param name="type">Type deriving from <see cref="Component"/> for which the custom handle will be displayed.
        ///                    Type can be null in which case the handle is always displayed.</param>
        public CustomHandle(Type type)
        {
            this.type = type;
        }
    }
}
