using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEngine
{
    public sealed class SerializableList : ScriptObject
    {
        private object referencedObject;
        private SerializableField.FieldType elementType;
        private Type internalElementType;
        private int[] dimensions;

        public SerializableArray(object obj)
        {
            Internal_CreateInstance(this, obj);

            referencedObject = obj;
        }

        public int GetDimension(int rank)
        {
            return dimensions[rank];
        }

        public SerializableField.FieldType ElementType
        {
            get { return elementType; }
        }

        public int Rank
        {
            get { return rank; }
        }

        public T GetValue<T>(params int[] indexes)
        {
            if (typeof(T) != internalElementType)
                throw new Exception("Attempted to retrieve a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalElementType);

            return (T)Internal_GetValue(mCachedPtr, ArrayIndexesToId(indexes));
        }

        public void SetValue<T>(T value, params int[] indexes)
        {
            if (typeof(T) != internalElementType)
                throw new Exception("Attempted to set a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalElementType);

            Internal_SetValue(mCachedPtr, ArrayIndexesToId(indexes), value);
        }

        private int ArrayIndexesToId(params int[] indexes)
        {
            int index = 0;
            int prevDimensionSize = 1;

            for (int i = dimensions.Length - 1; i >= 0; i--)
            {
                index += indexes[i] * prevDimensionSize;

                prevDimensionSize *= dimensions[i];
            }

            return index;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SerializableArray instance, object obj);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, int elementId, object value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern object Internal_GetValue(IntPtr nativeInstance, int elementId);
    }
}
