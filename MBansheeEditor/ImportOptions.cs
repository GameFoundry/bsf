using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Base class for all import options. Import options allow control over how is a specific resource type imported.
    /// </summary>
    public class ImportOptions : ScriptObject
    {

    }

    /// <summary>
    /// Import options that provide various options for controlling how is a texture file imported.
    /// </summary>
    public class TextureImportOptions : ImportOptions
    {
        /// <summary>
        /// Creates new texture import options with default values.
        /// </summary>
        public TextureImportOptions()
        {
            Internal_CreateInstance(this);
        }

        /// <summary>
        /// Pixel format to import as.
        /// </summary>
        public PixelFormat Format
        {
            get { return Internal_GetPixelFormat(mCachedPtr); }
            set { Internal_SetPixelFormat(mCachedPtr, value); }
        }

        /// <summary>
        /// Determines whether the imported texture will have mipmaps generated.
        /// </summary>
        public bool GenerateMipmaps
        {
            get { return Internal_GetGenerateMipmaps(mCachedPtr); }
            set { Internal_SetGenerateMipmaps(mCachedPtr, value); }
        }

        /// <summary>
        /// Maximum mipmap level to generate, if mipmap generation is enabled.
        /// </summary>
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

    /// <summary>
    /// Import options that provide various options for controlling how is a font file imported.
    /// </summary>
    public class FontImportOptions : ImportOptions
    {
        /// <summary>
        /// Creates new font import options with default values.
        /// </summary>
        public FontImportOptions()
        {
            Internal_CreateInstance(this);
        }

        /// <summary>
        /// Font sizes in points that are to be imported.
        /// </summary>
        public UInt32[] FontSizes
        {
            get { return Internal_GetFontSizes(mCachedPtr); }
            set { Internal_SetFontSizes(mCachedPtr, value); }
        }

        /// <summary>
        /// Dots per inch resolution to use when rendering the characters into the texture.
        /// </summary>
        public UInt32 DPI
        {
            get { return Internal_GetDPI(mCachedPtr); }
            set { Internal_SetDPI(mCachedPtr, value); }
        }

        /// <summary>
        /// Determines should rendered characters be antialiased.
        /// </summary>
        public bool Antialiasing
        {
            get { return Internal_GetAntialiasing(mCachedPtr); }
            set { Internal_SetAntialiasing(mCachedPtr, value); }
        }

        /// <summary>
        /// Determines character ranges to import from the font. Ranges are defined as unicode numbers.
        /// </summary>
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

    /// <summary>
    /// Import options that provide various options for controlling how is a script file imported.
    /// </summary>
    public class ScriptCodeImportOptions : ImportOptions
    {
        /// <summary>
        /// Creates new script import options with default values.
        /// </summary>
        public ScriptCodeImportOptions()
        {
            Internal_CreateInstance(this);
        }

        /// <summary>
        /// Determines whether the script is editor-only or a normal game script. Editor scripts are compiled in a separate
        /// assembly and may reference editor specific functionality, but are not available in the final game code.
        /// </summary>
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
