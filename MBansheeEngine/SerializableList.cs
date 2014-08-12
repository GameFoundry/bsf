using System;
using System.Collections;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
#pragma warning disable 649
    public sealed class SerializableList : ScriptObject
    {
        private SerializableProperty.FieldType elementType;
        private Type internalElementType;
        private IList referencedList;

        public SerializableProperty.FieldType ElementType
        {
            get { return elementType; }
        }

        // Constructed from native code
        private SerializableList(IList list, Type internalElementType)
        {
            referencedList = list;
            this.internalElementType = internalElementType;
            elementType = SerializableProperty.DetermineFieldType(internalElementType);
        }

        public SerializableProperty GetProperty(int elementIdx)
        {
            SerializableProperty.Getter getter = () => referencedList[elementIdx];
            SerializableProperty.Setter setter = (object value) => referencedList[elementIdx] = value;

            SerializableProperty property = Internal_CreateProperty(mCachedPtr);
            property.Construct(ElementType, internalElementType, getter, setter);

            return property;
        }

        public int GetLength()
        {
            return referencedList.Count;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateProperty(IntPtr nativeInstance);
    }
}
