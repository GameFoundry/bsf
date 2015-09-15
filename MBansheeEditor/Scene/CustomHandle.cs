using System;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Attribute that can be added to types deriving from <see cref="BansheeEditor.Handle"/>. That handle implementation
    /// will then be used whenever a handle for the type specified in this attribute needs to be displayed.
    /// </summary>
    [AttributeUsage(AttributeTargets.Class)]
    public sealed class CustomHandle : Attribute
    {
        private Type type;

        /// <summary>
        /// Creates a new custom handle attribute.
        /// </summary>
        /// <param name="type">Type deriving from <see cref="Component"/> for which the custom handle will be displayed.
        ///                    </param>
        public CustomHandle(Type type)
        {
            this.type = type;
        }
    }
}
