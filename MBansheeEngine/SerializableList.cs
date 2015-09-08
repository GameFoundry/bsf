using System;
using System.Collections;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
#pragma warning disable 649

    /// <summary>
    /// Allows you to access meta-data about a managed list and its children. Similar to Reflection but simpler and faster.
    /// </summary>
    public sealed class SerializableList : ScriptObject
    {
        private SerializableProperty.FieldType elementType;
        private Type internalElementType;
        private SerializableProperty parentProperty;

        /// <summary>
        /// Type of elements stored in the list.
        /// </summary>
        public SerializableProperty.FieldType ElementType
        {
            get { return elementType; }
        }

        /// <summary>
        /// Constructor for use by the runtime only.
        /// </summary>
        /// <param name="internalElementType">C# type of the elements in the list.</param>
        /// <param name="parentProperty">Property used for retrieving this entry.</param>
        private SerializableList(Type internalElementType, SerializableProperty parentProperty)
        {
            this.parentProperty = parentProperty;
            this.internalElementType = internalElementType;
            elementType = SerializableProperty.DetermineFieldType(internalElementType);
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
            property.Construct(ElementType, internalElementType, getter, setter);

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
}
