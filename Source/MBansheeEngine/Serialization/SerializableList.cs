//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Collections;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @addtogroup Serialization
     *  @{
     */

    #pragma warning disable 649

    /// <summary>
    /// Allows you to access meta-data about a managed list and its children. Similar to Reflection but simpler and faster.
    /// </summary>
    public sealed class SerializableList : ScriptObject
    {
        private SerializableProperty.FieldType elementPropertyType;
        private Type elementType;
        private SerializableProperty parentProperty;

        /// <summary>
        /// Type of serializable property used for the elements stored in the list.
        /// </summary>
        public SerializableProperty.FieldType ElementPropertyType
        {
            get { return elementPropertyType; }
        }

        /// <summary>
        /// Type of elements stored in the list.
        /// </summary>
        public Type ElementType
        {
            get { return elementType; }
        }

        /// <summary>
        /// Constructor for use by the runtime only.
        /// </summary>
        /// <param name="elementType">C# type of the elements in the list.</param>
        /// <param name="parentProperty">Property used for retrieving this entry.</param>
        private SerializableList(Type elementType, SerializableProperty parentProperty)
        {
            this.parentProperty = parentProperty;
            this.elementType = elementType;
            elementPropertyType = SerializableProperty.DetermineFieldType(elementType);
        }

        /// <summary>
        /// Returns a serializable property for a specific list element.
        /// </summary>
        /// <param name="elementIdx">Index of the element in the list.</param>
        /// <returns>Serializable property that allows you to manipulate contents of the list entry.</returns>
        public SerializableProperty GetProperty(int elementIdx)
        {
            SerializableProperty.Getter getter = () =>
            {
                IList list = parentProperty.GetValue<IList>();

                if (list != null)
                    return list[elementIdx];
                else
                    return null;
            };

            SerializableProperty.Setter setter = (object value) =>
            {
                IList list = parentProperty.GetValue<IList>();

                if (list != null)
                    list[elementIdx] = value;
            };

            SerializableProperty property = Internal_CreateProperty(mCachedPtr);
            property.Construct(ElementPropertyType, elementType, getter, setter);

            return property;
        }

        /// <summary>
        /// Returns number of elements in the list.
        /// </summary>
        /// <returns>Number of elements in the list.</returns>
        public int GetLength()
        {
            IList list = parentProperty.GetValue<IList>();

            if (list != null)
                return list.Count;
            else
                return 0;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateProperty(IntPtr nativeInstance);
    }

    /** @} */
}
