using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEngine
{
    #pragma warning disable 649
    public sealed class SerializableObject : ScriptObject
    {
        internal object referencedObject;
        private SerializableField[] _fields;

        public SerializableObject(object obj)
        {
            Internal_CreateInstance(this, obj);

            referencedObject = obj;
        }

        public SerializableField[] fields
        {
            get { return _fields; }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SerializableObject instance, object obj);
    }
}
