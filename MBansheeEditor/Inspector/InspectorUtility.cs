using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public class InspectorUtility
    {
        public static Inspector GetInspector(Type type)
        {
            Inspector customInspector = Internal_GetCustomInspector(type);
            if (customInspector != null)
                return customInspector;

            return new GenericInspector();
        }

        public static Type GetCustomInspectable(Type type)
        {
            Type customInspectable = Internal_GetCustomInspectable(type);
            if (customInspectable != null)
                return customInspectable;

            return null;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Inspector Internal_GetCustomInspector(Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Type Internal_GetCustomInspectable(Type type);
    }
}
