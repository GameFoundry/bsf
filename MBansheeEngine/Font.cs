using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /// <summary>
    /// Marks a range of characters in a font.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CharRange
    {
        public UInt32 start;
        public UInt32 end;
    }

    /// <summary>
    /// Font resource containing data about textual characters and how to render text.
    /// </summary>
    public sealed class Font : Resource // TODO - Dummy class
    {
        /// <summary>
        /// Creates a new font resource. For runtime use only.
        /// </summary>
        /// <param name="constructNative">Should the constructor also create the native interop object.</param>
        internal Font(bool constructNative)
        {
            if (constructNative)
                Internal_CreateInstance(this);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Font instance);
    }
}
