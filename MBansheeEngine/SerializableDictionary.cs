using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
#pragma warning disable 649
    public sealed class SerializableDictionary : ScriptObject
    {
        private SerializableProperty.FieldType keyType;
        private SerializableProperty.FieldType valueType;
        private Type internalKeyType;
        private Type internalValueType;
        private SerializableProperty parentProperty;

        public SerializableProperty.FieldType KeyType
        {
            get { return keyType; }
        }

        public SerializableProperty.FieldType ValueType
        {
            get { return valueType; }
        }

        // Constructed from native code
        private SerializableDictionary(Type internalKeyType, Type internalValueType, SerializableProperty parentProperty)
        {
            this.parentProperty = parentProperty;
            this.internalKeyType = internalKeyType;
            this.internalValueType = internalValueType;
            keyType = SerializableProperty.DetermineFieldType(internalKeyType);
            valueType = SerializableProperty.DetermineFieldType(internalValueType);
        }

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
