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
        private IDictionary referencedDict;

        public SerializableProperty.FieldType KeyType
        {
            get { return keyType; }
        }

        public SerializableProperty.FieldType ValueType
        {
            get { return valueType; }
        }

        // Constructed from native code
        private SerializableDictionary(IDictionary dict, Type internalKeyType, Type internalValueType)
        {
            referencedDict = dict;
            this.internalKeyType = internalKeyType;
            this.internalValueType = internalValueType;
            keyType = SerializableProperty.DetermineFieldType(internalKeyType);
            valueType = SerializableProperty.DetermineFieldType(internalValueType);
        }

        public KeyValuePair<SerializableProperty, SerializableProperty> GetProperty(object key)
        {
            if (!referencedDict.Contains(key))
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
                SerializableProperty.Getter getter = () => referencedDict[key];
                SerializableProperty.Setter setter = (object value) => referencedDict[key] = value;

                valueProperty = Internal_CreateValueProperty(mCachedPtr);
                valueProperty.Construct(ValueType, internalValueType, getter, setter);
            }

            return new KeyValuePair<SerializableProperty, SerializableProperty>(keyProperty, valueProperty);
        }

        public int GetLength()
        {
            return referencedDict.Count;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateKeyProperty(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateValueProperty(IntPtr nativeInstance);
    }
}
