using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEditor
{
    public class SerializableField
    {
        public enum FieldType
        {
            Int,
            Float,
            Bool,
            String,
            Color,
            Vector2,
            Vector3,
            Vector4,
            GameObjectRef,
            ResourceRef,
            Object,
            Array,
            List,
            Dictionary
        }

        private SerializableObject parent;
        private FieldType type;
        private int fieldId;
        private string name;

        internal SerializableField(SerializableObject parent, FieldType type, int fieldId)
        {
            this.parent = parent;
            this.type = type;
            this.fieldId = fieldId;
        }

        public FieldType Type
        {
            get { return type; }
        }

        public string Name
        {
            get { return name; }
        }

        public SerializableValue GetValue()
        {
            return null; // TODO - Return actual SerializableValue
        }

        // TODO - Add getters/setters for all fields

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetInt32(IntPtr nativeInstance, int fieldId, Int32 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Int32 Internal_GetInt32(IntPtr nativeInstance, int fieldId);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetObject(IntPtr nativeInstance, int fieldId, object value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern object Internal_GetObject(IntPtr nativeInstance, int fieldId);
    }
}
