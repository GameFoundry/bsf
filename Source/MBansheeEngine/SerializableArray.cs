//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    #pragma warning disable 649

    /// <summary>
    /// Allows you to access meta-data about a managed array and its children. Similar to Reflection but simpler and faster.
    /// </summary>
    public sealed class SerializableArray : ScriptObject
    {
        private SerializableProperty.FieldType elementPropertyType;
        private Type elementType;
        private SerializableProperty parentProperty;

        /// <summary>
        /// Type of serializable property used for the elements stored in the array.
        /// </summary>
        public SerializableProperty.FieldType ElementPropertyType
        {
            get { return elementPropertyType; }
        }

        /// <summary>
        /// Type used for the elements stored in the array.
        /// </summary>
        public Type ElementType
        {
            get { return elementType; }
        }

        /// <summary>
        /// Constructor for use by the runtime only.
        /// </summary>
        /// <param name="elementType">C# type of the elements in the array.</param>
        /// <param name="parentProperty">Property used for retrieving this entry.</param>
        private SerializableArray(Type elementType, SerializableProperty parentProperty)
        {
            this.parentProperty = parentProperty;
            this.elementType = elementType;
            elementPropertyType = SerializableProperty.DetermineFieldType(elementType);
        }

        /// <summary>
        /// Returns a serializable property for a specific array element.
        /// </summary>
        /// <param name="elementIdx">Index of the element in the array.</param>
        /// <returns>Serializable property that allows you to manipulate contents of the array entry.</returns>
        public SerializableProperty GetProperty(int elementIdx)
        {
            SerializableProperty.Getter getter = () =>
            {
                Array array = parentProperty.GetValue<Array>();

                if (array != null)
                    return array.GetValue(elementIdx);
                else
                    return null;
            };

            SerializableProperty.Setter setter = (object value) =>
            {
                Array array = parentProperty.GetValue<Array>();

                if(array != null)
                    array.SetValue(value, elementIdx);
            };

            SerializableProperty property = Internal_CreateProperty(mCachedPtr);
            property.Construct(ElementPropertyType, elementType, getter, setter);

            return property;
        }

        /// <summary>
        /// Returns number of elements in the array.
        /// </summary>
        /// <returns>Number of elements in the array.</returns>
        public int GetLength()
        {
            Array array = parentProperty.GetValue<Array>();

            if (array != null)
                return array.GetLength(0); // TODO - Support multi-rank arrays
            else
                return 0;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateProperty(IntPtr nativeInstance);
    }
}
