using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class SerializableField : ScriptObject
    {
        private SerializableObject parent;
        private SerializableProperty.FieldType type;
        private int flags;
        private Type internalType;
        private string name;

        // Only constructed from native code
        private SerializableField(SerializableObject parent, string name, int flags, Type internalType)
        {
            this.parent = parent;
            this.name = name;
            this.flags = flags;
            this.type = SerializableProperty.DetermineFieldType(internalType);
            this.internalType = internalType;
        }

        public SerializableProperty.FieldType Type
        {
            get { return type; }
        }

        public bool HasCustomInspector
        {
            get { return false; } // TODO - Add [UseCustomInspector(typeof(InspecableType))] attribute and parse it
        }

        public Type CustomInspectorType
        {
            get { return null; } // TODO - See above. Return type from UseCustomInspector attribute
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

        public SerializableProperty GetProperty()
        {
            SerializableProperty.Getter getter = () =>
            {
                object parentObject = parent.GetReferencedObject();
                
                if (parentObject != null)
                    return Internal_GetValue(mCachedPtr, parentObject);
                else
                    return null;
            };

            SerializableProperty.Setter setter = (object value) =>
            {
                object parentObject = parent.GetReferencedObject();

                if (parentObject != null)
                {
                    Internal_SetValue(mCachedPtr, parentObject, value);

                    // If value type we cannot just modify the parent object because it's just a copy
                    if (parentObject.GetType().IsValueType && parent.parentProperty != null)
                        parent.parentProperty.SetValue(parentObject);
                }
            };

            SerializableProperty newProperty = Internal_CreateProperty(mCachedPtr);
            newProperty.Construct(type, internalType, getter, setter);

            return newProperty;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern SerializableProperty Internal_CreateProperty(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern object Internal_GetValue(IntPtr nativeInstance, object instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, object instance, object value);
    }
}
