//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Base class for all textures. Contains a set of pixels of certain dimensions that can be used for rendering
    /// or read/written directly.
    /// </summary>
    public class Texture : Resource
    {
        /// <summary>
        /// Returns the pixel format for the texture surface.
        /// </summary>
        public PixelFormat PixelFormat
        {
            get
            {
                PixelFormat value;
                Internal_GetPixelFormat(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Returns a value that signals the engine in what way is the texture expected to be used.
        /// </summary>
        public TextureUsage Usage
        {
            get
            {
                TextureUsage value;
                Internal_GetUsage(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Width of the texture in pixels.
        /// </summary>
        public int Width
        {
            get
            {
                int value;
                Internal_GetWidth(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Height of the texture in pixels.
        /// </summary>
        public int Height
        {
            get
            {
                int value;
                Internal_GetHeight(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Determines does the texture contain gamma corrected data. If true then the GPU will automatically convert
        /// the pixels to linear space before reading from the texture, and convert them to gamma space when writing
        /// to the texture.
        /// </summary>
        public bool GammaCorrection
        {
            get
            {
                bool value;
                Internal_GetGammaCorrection(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Number of samples per pixel. Zero or one mean no multisampling will be used.
        /// </summary>
        public int SampleCount
        {
            get
            {
                int value;
                Internal_GetSampleCount(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Returns how many mipmap levels does the texture contain.
        /// </summary>
        public int MipmapCount
        {
            get
            {
                int value;
                Internal_GetMipmapCount(mCachedPtr, out value);
                return value;
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPixelFormat(IntPtr thisPtr, out PixelFormat value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetUsage(IntPtr thisPtr, out TextureUsage value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetWidth(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetHeight(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetGammaCorrection(IntPtr thisPtr, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetSampleCount(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetMipmapCount(IntPtr thisPtr, out int value);
    }

    /// <summary>
    /// Flags that describe how is a texture used.
    /// </summary>
    public enum TextureUsage // Note: Must match C++ enum TextureUsage
    {
        /// <summary>
        /// A regular texture that is not often or ever updated from the CPU.
        /// </summary>
        Default = 0x1,

        /// <summary>
        /// A regular texture that is often updated by the CPU.
        /// </summary>
        Dynamic = 0x2,

        /// <summary>
        /// Texture that can be rendered to by the GPU.
        /// </summary>
        Render = 0x200,

        /// <summary>
        /// Texture used as a depth/stencil buffer by the GPU.
        /// </summary>
        DepthStencil = 0x400,

        /// <summary>
        /// Texture that allows load/store operations from the GPU program.
        /// </summary>
        LoadStore = 0x800,

        /// <summary>
        /// Ensures all texture data will also be cached in system memory so it can be read by the CPU.
        /// </summary>
        CPUCached = 0x1000
    }
}
