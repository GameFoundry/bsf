using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    #pragma warning disable 649

    /// <summary>
    /// Allows you to access meta-data about a managed dictionary and its children. Similar to Reflection but simpler and 
    /// faster.
    /// </summary>
    public sealed class SerializableDictionary : ScriptObject
    {
        private SerializableProperty.FieldType keyType;
        private SerializableProperty.FieldType valueType;
        private Type internalKeyType;
        private Type internalValueType;
        private SerializableProperty parentProperty;

        /// <summary>
        /// Type of keys stored in the dictionary.
        /// </summary>
        public SerializableProperty.FieldType KeyType
        {
            get { return keyType; }
        }

        /// <summary>
        /// Type of values stored in the dictionary.
        /// </summary>
        public SerializableProperty.FieldType ValueType
        {
            get { return valueType; }
        }

        /// <summary>
        /// Constructor for use by the runtime only.
        /// </summary>
        /// <param name="internalKeyType">C# type of the keys in the dictionary.</param>
        /// <param name="internalValueType">C# type of the values in the dictionary.</param>
        /// <param name="parentProperty">Property this dictionary belongs to.</param>
        private SerializableDictionary(Type internalKeyType, Type internalValueType, SerializableProperty parentProperty)
        {
            this.parentProperty = parentProperty;
            this.internalKeyType = internalKeyType;
            this.internalValueType = internalValueType;
            keyType = SerializableProperty.DetermineFieldType(internalKeyType);
            valueType = SerializableProperty.DetermineFieldType(internalValueType);
        }

        /// <summary>
        /// Returns a property that can be used for retrieving meta-data about a value in the dictionary.
        /// </summary>
        /// <param name="key">Dictionary key for the value to retrieve.</param>
        /// <returns>Serializable property for the element that may be used for querying element meta-data.</returns>
        public KeyValuePair<SerializableProperty, SerializableProperty> GetProperty(object key)
        {
            IDictionary dictionary = parentProperty.GetValue<IDictionary>();

            if (dictionary == null || !dictionary.Contains(key))
                return new KeyValuePair<SerializableProperty, SerializableProperty>(null, null);

            SerializableProperty keyProperty;
            {
                SerializableProperty.Getter getter = () => key;
                SerializableProperty.Setter setter = (object value) => {};

                keyProperty = Internal_CreateKeyProperty(mCachedPtr);
                keyProperty.Construct(KeyType, internalKeyType, getter, setter);
            }

            SerializableProperty valueProperty;
            {
                SerializableProperty.Getter getter = () =>
                {
                    IDictionary dict = parentProperty.GetValue<IDictionary>();

                    if (dict != null)
                        return dict[key];
                    else
                        return null;
                };

                SerializableProperty.Setter setter = (object value) =>
                {
                    IDictionary dict = parentProperty.GetValue<IDictionary>();

                    if (dict != null)
                        dict[key] = value;
                };

                valueProperty = Internal_CreateValueProperty(mCachedPtr);
                valueProperty.Construct(ValueType, internalValueType, getter, setter);
            }

            return new KeyValuePair<SerializableProperty, SerializableProperty>(keyProperty, valueProperty);
        }

        /// <summary>
        /// Returns the total number of elements in the dictionary.
        /// </summary>
        /// <returns>Total number of elements in the dictionary.</returns>
        public int GetLength()
        {
            IDictionary dictionary = parentProperty.GetValue<IDictionary>();

            if (dictionary != null)
                return dictionary.Count;
            else
                return 0;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateKeyProperty(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateValueProperty(IntPtr nativeInstance);
    }
}
