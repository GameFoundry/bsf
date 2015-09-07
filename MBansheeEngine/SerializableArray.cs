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
        private SerializableProperty.FieldType elementType;
        private Type internalElementType;
        private SerializableProperty parentProperty;

        /// <summary>
        /// Type of elements stored in the array.
        /// </summary>
        public SerializableProperty.FieldType ElementType
        {
            get { return elementType; }
        }

        /// <summary>
        /// Constructor for use by the runtime only.
        /// </summary>
        /// <param name="internalElementType">C# type of the elements in the array.</param>
        /// <param name="parentProperty">Property this array belongs to.</param>
        private SerializableArray(Type internalElementType, SerializableProperty parentProperty)
        {
            this.parentProperty = parentProperty;
            this.internalElementType = internalElementType;
            elementType = SerializableProperty.DetermineFieldType(internalElementType);
        }

        /// <summary>
        /// Returns a serializable property for a specific array element.
        /// </summary>
        /// <param name="elementIdx">Index of the element in the array.</param>
        /// <returns>Serializable property for the element that may be used for querying element meta-data.</returns>
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
            property.Construct(ElementType, internalElementType, getter, setter);

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
