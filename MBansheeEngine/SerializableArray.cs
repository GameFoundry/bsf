using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    #pragma warning disable 649
    public sealed class SerializableArray : ScriptObject
    {
        private SerializableField.FieldType elementType;
        private Type internalElementType;
        private Array referencedArray;

        public SerializableField.FieldType ElementType
        {
            get { return elementType; }
        }

        internal SerializableArray(Array array)
        {
            referencedArray = array;
            Internal_CreateInstance(this, referencedArray);
        }

        public SerializableProperty GetProperty(int elementIdx)
        {
            SerializableProperty.Getter getter = () => referencedArray.GetValue(elementIdx);
            SerializableProperty.Setter setter = (object value) => referencedArray.SetValue(value, elementIdx);

            return new SerializableProperty(ElementType, internalElementType, getter, setter);
        }

        public int GetLength()
        {
            return referencedArray.GetLength(0); // TODO - Support multi-rank arrays
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SerializableArray instance, object obj);
    }
}
