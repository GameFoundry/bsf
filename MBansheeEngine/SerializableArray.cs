using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEngine
{
    public sealed class SerializableArray
    {
        public SerializableArray(object obj)
        {
            // TODO - Initialize the array - handle it properly in case obj isn't a valid array
        }

        public SerializableField.FieldType ElementType;
        private int[] dimensions;
        private int rank;

        public int GetDimension(int rank)
        {
            return dimensions[rank];
        }

        public int Rank
        {
            get { return rank;  }
        }

        public SerializableValue GetValue(int id)
        {
            return null; // TODO - Return actual SerializableValue
        }

        // TODO - Add getters/setters for all fields

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetInt32(IntPtr nativeInstance, int arrayIdx, Int32 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Int32 Internal_GetInt32(IntPtr nativeInstance, int arrayIdx);
    }
}
