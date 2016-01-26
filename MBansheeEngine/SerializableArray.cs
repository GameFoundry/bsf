using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    #pragma warning disable 649
    public sealed class SerializableArray : ScriptObject
    {
        private SerializableProperty.FieldType elementType;
        private Type internalElementType;
        private Array referencedArray;

        public SerializableProperty.FieldType ElementType
        {
            get { return elementType; }
        }

        // Constructed from native code
        private SerializableArray(Array array, Type internalElementType)
        {
            referencedArray = array;
            this.internalElementType = internalElementType;
            elementType = SerializableProperty.DetermineFieldType(internalElementType);
        }

        public SerializableProperty GetProperty(int elementIdx)
        {
            SerializableProperty.Getter getter = () => referencedArray.GetValue(elementIdx);
            SerializableProperty.Setter setter = (object value) => referencedArray.SetValue(value, elementIdx);

            SerializableProperty property = Internal_CreateProperty(mCachedPtr);
            property.Construct(ElementType, internalElementType, getter, setter);

            return property;
        }

        public int GetLength()
        {
            return referencedArray.GetLength(0); // TODO - Support multi-rank arrays
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateProperty(IntPtr nativeInstance);
    }
}
