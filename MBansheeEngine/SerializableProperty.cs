using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public sealed class SerializableProperty : ScriptObject
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

        internal delegate object Getter();
        internal delegate void Setter(object value);

        private FieldType type;
        private Type internalType;
        private Getter getter;
        private Setter setter;

        // Constructed from native code
        private SerializableProperty()
        { }

        internal void Construct(FieldType type, Type internalType, Getter getter, Setter setter)
        {
            this.type = type;
            this.internalType = internalType;
            this.getter = getter;
            this.setter = setter;
        }

        public FieldType Type
        {
            get { return type; }
        }

        public T GetValue<T>()
        {
            if (!typeof(T).IsAssignableFrom(internalType))
                throw new Exception("Attempted to retrieve a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalType);

            return (T)getter();
        }

        public T GetValueCopy<T>()
        {
            if (!typeof(T).IsAssignableFrom(internalType))
                throw new Exception("Attempted to retrieve a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalType);

            return (T)Internal_CloneManagedInstance(mCachedPtr, getter());
        }

        public void SetValue<T>(T value)
        {
            if (!typeof(T).IsAssignableFrom(internalType))
                throw new Exception("Attempted to set a serializable value using an invalid type. Provided type: " + typeof(T) + ". Needed type: " + internalType);

            setter(value);
        }

        public SerializableObject GetObject()
        {
            if (type != FieldType.Object)
                throw new Exception("Attempting to retrieve object information from a field that doesn't contain an object.");

            return Internal_CreateObject(mCachedPtr, GetValue<object>());
        }

        public SerializableArray GetArray()
        {
            if (type != FieldType.Array)
                throw new Exception("Attempting to retrieve array information from a field that doesn't contain an array.");

            return Internal_CreateArray(mCachedPtr, GetValue<Array>());
        }

        public T CreateObjectInstance<T>()
        {
            if (type != FieldType.Object)
                throw new Exception("Attempting to retrieve object information from a field that doesn't contain an object.");

            return (T) Internal_CreateMangedObjectInstance(mCachedPtr);
        }

        public Array CreateArrayInstance(int[] lengths)
        {
            if (type != FieldType.Array)
                throw new Exception("Attempting to retrieve array information from a field that doesn't contain an array.");

            return Internal_CreateManagedArrayInstance(mCachedPtr, lengths);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableObject Internal_CreateObject(IntPtr nativeInstance, object instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableArray Internal_CreateArray(IntPtr nativeInstance, Array instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern object Internal_CreateMangedObjectInstance(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Array Internal_CreateManagedArrayInstance(IntPtr nativeInstance, int[] lengths);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern object Internal_CloneManagedInstance(IntPtr nativeInstance, object original);

        public static FieldType DetermineFieldType(Type internalType)
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
    }
}
