using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class SerializableDictionaryInfo : ScriptObject
    {
        private SerializableField.FieldType keyType;
        private SerializableField.FieldType valueType;

        public SerializableField.FieldType KeyType
        {
            get { return keyType; }
        }

        public SerializableField.FieldType ValueType
        {
            get { return valueType; }
        }

        public SerializableDictionaryInfo(object obj)
        {
            Internal_CreateInstance(this, obj);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(SerializableDictionaryInfo instance, object obj);
    }
}
