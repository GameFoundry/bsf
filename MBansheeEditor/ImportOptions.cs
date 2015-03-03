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

    public class GpuProgramImportOptions : ImportOptions
    {
        public GpuProgramImportOptions()
        {
            Internal_CreateInstance(this);
        }

        public string EntryPoint
        {
            get { return Internal_GetEntryPoint(mCachedPtr); }
            set { Internal_SetEntryPoint(mCachedPtr, value); }
        }

        public GpuLanguage Language
        {
            get { return Internal_GetLanguage(mCachedPtr); }
            set { Internal_SetLanguage(mCachedPtr, value); }
        }

        public GpuProgramProfile Profile
        {
            get { return Internal_GetProfile(mCachedPtr); }
            set { Internal_SetProfile(mCachedPtr, value); }
        }

        public GpuProgramType Type
        {
            get { return Internal_GetType(mCachedPtr); }
            set { Internal_SetType(mCachedPtr, value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GpuProgramImportOptions instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string Internal_GetEntryPoint(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetEntryPoint(IntPtr thisPtr, string value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GpuLanguage Internal_GetLanguage(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLanguage(IntPtr thisPtr, GpuLanguage value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GpuProgramProfile Internal_GetProfile(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetProfile(IntPtr thisPtr, GpuProgramProfile value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GpuProgramType Internal_GetType(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetType(IntPtr thisPtr, GpuProgramType value);
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
}
