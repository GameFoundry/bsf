using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEditor
{
    public class InspectableField
    {
        public enum Type
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

        private EditorField guiField; // TODO - This will likely be a specific EditorField type, not a generic base one
        private InspectableObjectBase parent;
        private Type type;

        // TODO - Add getters/setters for all fields
        
        // TODO - Make sure "guiField" is created properly

        public void Refresh()
        {
            // TODO - Update "guiField" from the field value by calling "Internal_Get*" method
        }

        public void Destroy()
        {
            // TODO - Called by the parent
            //  Release "guiField"
        }

        private void OnValueChangedInt32(Int32 newValue)
        {
            // TODO - Callback from "guiField"
            //  Need separate methods for all types
            //  Call "Internal_Set*" method to update the actual field
            //  Register an "Undo" action
        }

        // TODO - I need a set of these methods for all possible "Type"s
        //  Internally they should use SerializableField methods for setting/getting values

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetInt32(IntPtr nativeInstance, Int32 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Int32 Internal_GetInt32(IntPtr nativeInstance);
    }
}
