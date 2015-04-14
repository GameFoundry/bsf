using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    public class ImportOptions : ScriptObject
    {

    }

    public class TextureImportOptions : ImportOptions
    {
        public TextureImportOptions()
        {
            Internal_CreateInstance(this);
        }

        public PixelFormat Format
        {
            get { return Internal_GetPixelFormat(mCachedPtr); }
            set { Internal_SetPixelFormat(mCachedPtr, value); }
        }

        public bool GenerateMipmaps
        {
            get { return Internal_GetGenerateMipmaps(mCachedPtr); }
            set { Internal_SetGenerateMipmaps(mCachedPtr, value); }
        }

        public UInt32 MaxMipmapLevel
        {
            get { return Internal_GetMaxMipmapLevel(mCachedPtr); }
            set { Internal_SetMaxMipmapLevel(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(TextureImportOptions instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PixelFormat Internal_GetPixelFormat(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPixelFormat(IntPtr thisPtr, PixelFormat value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetGenerateMipmaps(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetGenerateMipmaps(IntPtr thisPtr, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt32 Internal_GetMaxMipmapLevel(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMaxMipmapLevel(IntPtr thisPtr, UInt32 value);
    }

    public class FontImportOptions : ImportOptions
    {
        public FontImportOptions()
        {
            Internal_CreateInstance(this);
        }

        public UInt32[] FontSizes
        {
            get { return Internal_GetFontSizes(mCachedPtr); }
            set { Internal_SetFontSizes(mCachedPtr, value); }
        }

        public UInt32 DPI
        {
            get { return Internal_GetDPI(mCachedPtr); }
            set { Internal_SetDPI(mCachedPtr, value); }
        }

        public bool Antialiasing
        {
            get { return Internal_GetAntialiasing(mCachedPtr); }
            set { Internal_SetAntialiasing(mCachedPtr, value); }
        }

        public CharRange[] CharRanges
        {
            get { return Internal_GetCharRanges(mCachedPtr); }
            set { Internal_SetCharRanges(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(FontImportOptions instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt32[] Internal_GetFontSizes(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFontSizes(IntPtr thisPtr, UInt32[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt32 Internal_GetDPI(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetDPI(IntPtr thisPtr, UInt32 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetAntialiasing(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetAntialiasing(IntPtr thisPtr, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern CharRange[] Internal_GetCharRanges(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCharRanges(IntPtr thisPtr, CharRange[] value);
    }

    public class ScriptCodeImportOptions : ImportOptions
    {
        public ScriptCodeImportOptions()
        {
            Internal_CreateInstance(this);
        }

        public bool EditorScript
        {
            get { return Internal_IsEditorScript(mCachedPtr); }
            set { Internal_SetEditorScript(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(ScriptCodeImportOptions instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsEditorScript(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEditorScript(IntPtr thisPtr, bool value);
    }
}
