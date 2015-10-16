using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Contains utility methods relating to inspector window.
    /// </summary>
    public class InspectorUtility
    {
        /// <summary>
        /// Creates an inspector capable of displaying GUI elements for an object of the provided type.
        /// </summary>
        /// <param name="type">Type of the object that will be displayed in the inspector.</param>
        /// <returns>Custom user defined inspector if it exists for the provided type, or the generic inspector.</returns>
        public static Inspector GetInspector(Type type)
        {
            Inspector customInspector = Internal_GetCustomInspector(type);
            if (customInspector != null)
                return customInspector;

            return new GenericInspector();
        }

        /// <summary>
        /// Gets an <see cref="InspectableField"/> implementation for the specified type. This only searches custom user
        /// defined implementations, not the built-in ones.
        /// </summary>
        /// <param name="type">Type of the object to find an <see cref="InspectableField"/> for.</param>
        /// <returns>Implementation of <see cref="InspectableField"/> capable of display contents of the provided type,
        ///          or null if one wasn't found.</returns>
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

    /// <summary>
    /// States an inspectable object can be in. Higher states override lower states.
    /// </summary>
    [Flags]
    public enum InspectableState
    {
        /// <summary>Object was not modified this frame.</summary>
        NotModified,
        /// <summary>Object was modified but was not confirmed, therefore does not require saving.</summary>
        Modified = 1,
        /// <summary>Object was modified and confirmed, therefore it requires saving.</summary>
        ModifiedConfirm = 2
    }
}
