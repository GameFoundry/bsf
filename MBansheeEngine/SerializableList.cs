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
        private SerializableProperty parentProperty;

        public SerializableProperty.FieldType ElementType
        {
            get { return elementType; }
        }

        // Constructed from native code
        private SerializableList(Type internalElementType, SerializableProperty parentProperty)
        {
            this.parentProperty = parentProperty;
            this.internalElementType = internalElementType;
            elementType = SerializableProperty.DetermineFieldType(internalElementType);
        }

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
