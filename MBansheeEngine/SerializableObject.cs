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

        public SerializableObject(Type objectType, object instance)
        {
            Internal_CreateInstance(this, objectType, instance);

            referencedObject = instance;
        }

        // Constructed from native code
        private SerializableObject(object instance)
        {
            referencedObject = instance;
        }

        public SerializableField[] fields
        {
            get { return _fields; }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SerializableObject instance, Type objectType, object objInstance);
    }
}
