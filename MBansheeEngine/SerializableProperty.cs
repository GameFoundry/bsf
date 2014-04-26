using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BansheeEngine
{
    public sealed class SerializableProperty
    {
        internal delegate object Getter();
        internal delegate void Setter(object value);

        private SerializableField.FieldType type;
        private Type internalType;
        private Getter getter;
        private Setter setter;

        internal SerializableProperty(SerializableField.FieldType type, Type internalType, Getter getter, Setter setter)
        {
            this.type = type;
            this.internalType = internalType;
            this.getter = getter;
            this.setter = setter;
        }

        public SerializableField.FieldType Type
        {
            get { return type; }
        }

        public T GetValue<T>()
        {
            if (!typeof(T).IsAssignableFrom(internalType))
                throw new Exception("Attempted to retrieve a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalType);

            return (T)getter();
        }

        public void SetValue<T>(T value)
        {
            if (!typeof(T).IsAssignableFrom(internalType))
                throw new Exception("Attempted to set a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalType);

            setter(value);
        }

        public SerializableObject GetObject()
        {
            if (type != SerializableField.FieldType.Object)
                throw new Exception("Attempting to retrieve object information from a field that doesn't contain an object.");

            return new SerializableObject(GetValue<object>());
        }

        public SerializableArray GetArray()
        {
            if (type != SerializableField.FieldType.Array)
                throw new Exception("Attempting to retrieve array information from a field that doesn't contain an array.");

            return new SerializableArray(GetValue<Array>());
        }
    }
}
