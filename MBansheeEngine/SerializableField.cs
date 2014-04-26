using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class SerializableField : ScriptObject
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
        private int flags;
        private Type internalType;
        private string name;

        // Only constructed from native code
        private SerializableField(SerializableObject parent, string name, int flags, Type internalType)
        {
            this.parent = parent;
            this.name = name;
            this.flags = flags;
            this.type = DetermineFieldType(internalType);
            this.internalType = internalType;
        }

        public FieldType Type
        {
            get { return type; }
        }

        public string Name
        {
            get { return name; }
        }

        public bool Inspectable
        {
            get { return (flags & 0x02) != 0; } // Flags as defined in native code in BsManagedSerializableObjectInfo.h
        }

        public bool Serializable
        {
            get { return (flags & 0x01) != 0; } // Flags as defined in native code in BsManagedSerializableObjectInfo.h
        }

        private static FieldType DetermineFieldType(Type internalType)
        {
            if (!internalType.IsArray)
            {
                if (internalType == typeof (Byte))
                    return FieldType.Int;
                else if (internalType == typeof (SByte))
                    return FieldType.Int;
                else if (internalType == typeof (Int16))
                    return FieldType.Int;
                else if (internalType == typeof (UInt16))
                    return FieldType.Int;
                else if (internalType == typeof (Int32))
                    return FieldType.Int;
                else if (internalType == typeof (UInt32))
                    return FieldType.Int;
                else if (internalType == typeof (Int64))
                    return FieldType.Int;
                else if (internalType == typeof (UInt64))
                    return FieldType.Int;
                else if (internalType == typeof (bool))
                    return FieldType.Bool;
                else if (internalType == typeof (float))
                    return FieldType.Float;
                else if (internalType == typeof (double))
                    return FieldType.Float;
                else if (internalType == typeof (string))
                    return FieldType.String;
                else if (internalType == typeof (Vector2))
                    return FieldType.Vector2;
                else if (internalType == typeof (Vector3))
                    return FieldType.Vector3;
                else if (internalType == typeof (Vector4))
                    return FieldType.Vector4;
                else if (internalType == typeof (Color))
                    return FieldType.Color;
                else if (internalType.IsSubclassOf(typeof (GameObject)))
                    return FieldType.GameObjectRef;
                else if (internalType.IsSubclassOf(typeof (Resource)))
                    return FieldType.ResourceRef;
                else if (internalType.IsGenericType)
                {
                    Type genericType = internalType.GetGenericTypeDefinition();

                    if (genericType == typeof (List<>))
                    {
                        return FieldType.List;
                    }
                    else if (genericType == typeof (Dictionary<,>))
                    {
                        return FieldType.Dictionary;
                    }

                    // Shouldn't happen because native code should only supply us with supported types
                    throw new Exception("Cannot determine field type. Found an unsupported generic type.");
                }


                // Otherwise the type must be an object, unless some error occurred
                return FieldType.Object;
            }

            return FieldType.Array;
        }

        public T GetValue<T>()
        {
            if (!typeof(T).IsAssignableFrom(internalType))
                throw new Exception("Attempted to retrieve a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalType);

            return (T)Internal_GetValue(mCachedPtr, parent.referencedObject);
        }

        public void SetValue<T>(T value)
        {
            if (!typeof(T).IsAssignableFrom(internalType))
                throw new Exception("Attempted to set a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalType);

            Internal_SetValue(mCachedPtr, parent.referencedObject, value);
        }

        public SerializableArrayInfo GetSerializableArrayInfo()
        {
            if (type != FieldType.Array)
                throw new Exception("Attempting to retrieve array information from a field that doesn't contain an array.");

            return new SerializableArrayInfo(GetValue<object>());
        }

        public SerializableListInfo GetSerializableListInfo()
        {
            if (type != FieldType.List)
                throw new Exception("Attempting to retrieve array information from a field that doesn't contain an array.");

            return new SerializableListInfo(GetValue<object>());
        }

        public SerializableDictionaryInfo GetSerializableDictionaryInfo()
        {
            if (type != FieldType.Dictionary)
                throw new Exception("Attempting to retrieve array information from a field that doesn't contain an array.");

            return new SerializableDictionaryInfo(GetValue<object>());
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern object Internal_GetValue(IntPtr nativeInstance, object instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, object instance, object value);
    }
}
