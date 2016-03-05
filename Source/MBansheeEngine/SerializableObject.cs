//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEngine
{
    #pragma warning disable 649

    /// <summary>
    /// Allows you to access meta-data about a managed object and its fields. Similar to Reflection but simpler and faster.
    /// </summary>
    public sealed class SerializableObject : ScriptObject
    {
        internal SerializableProperty parentProperty;
        internal object parentObject;
        private SerializableField[] _fields;

        /// <summary>
        /// Creates a new serializable object for the specified object type.
        /// </summary>
        /// <param name="objectType">C# type of the object.</param>
        /// <param name="parentProperty">Property used for retrieving this entry.</param>
        public SerializableObject(Type objectType, SerializableProperty parentProperty)
        {
            Internal_CreateInstance(this, objectType);

            this.parentProperty = parentProperty;
            this.parentObject = null;
        }

        /// <summary>
        /// Creates a new serializable object for the specified object type.
        /// </summary>
        /// <param name="objectType">C# type of the object.</param>
        /// <param name="parentObject">Specific instance of the object of <paramref name="objectType"/>.</param>
        public SerializableObject(Type objectType, object parentObject)
        {
            Internal_CreateInstance(this, objectType);

            this.parentProperty = null;
            this.parentObject = parentObject;
        }

        /// <summary>
        /// Creates a new serializable object for the specified object. Object must not be null.
        /// </summary>
        /// <param name="parentObject">Specific instance of the object.</param>
        public SerializableObject(object parentObject)
        {
            Internal_CreateInstance(this, parentObject.GetType());

            this.parentProperty = null;
            this.parentObject = parentObject;
        }

        /// <summary>
        /// Returns all fields in the object.
        /// </summary>
        public SerializableField[] Fields
        {
            get { return _fields; }
        }

        /// <summary>
        /// Returns the specific object instance this object is operating on.
        /// </summary>
        /// <returns>Object instance.</returns>
        public object GetReferencedObject()
        {
            if (parentProperty != null)
                return parentProperty.GetValue<object>();
            else
                return parentObject;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SerializableObject instance, Type objectType);
    }
}
