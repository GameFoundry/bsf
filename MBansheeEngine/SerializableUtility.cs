using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Provides utility methods dealing with object serialization.
    /// </summary>
    public static class SerializableUtility
    {
        /// <summary>
        /// Clones the specified object. Non-serializable types and fields are ignored in clone. A deep copy is performed
        /// on all serializable elements except for resources or game objects.
        /// </summary>
        /// <param name="original">Non-null reference to the object to clone. Object type must be serializable.</param>
        /// <returns>Deep copy of the original object.</returns>
        public static object Clone(object original)
        {
            return Internal_Clone(original);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern object Internal_Clone(object original);
    }
}
