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
        internal SerializableProperty parentProperty;
        internal object parentObject;
        private SerializableField[] _fields;

        // Note: Also called from native code
        public SerializableObject(Type objectType, SerializableProperty parentProperty)
        {
            Internal_CreateInstance(this, objectType);

            this.parentProperty = parentProperty;
            this.parentObject = null;
        }

        public SerializableObject(Type objectType, object parentObject)
        {
            Internal_CreateInstance(this, objectType);

            this.parentProperty = null;
            this.parentObject = parentObject;
        }

        public SerializableField[] fields
        {
            get { return _fields; }
        }

        public object GetReferencedObject()
        {
            if (parentProperty != null)
                return parentProperty.GetValue<object>();
            else
                return parentObject;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SerializableObject instance, Type objectType);
    }
}
