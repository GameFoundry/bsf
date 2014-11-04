using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class Texture : Resource
    {
        public PixelFormat PixelFormat
        {
            get
            {
                PixelFormat value;
                Internal_GetPixelFormat(mCachedPtr, out value);
                return value;
            }
        }

        public TextureUsage Usage
        {
            get
            {
                TextureUsage value;
                Internal_GetUsage(mCachedPtr, out value);
                return value;
            }
        }

        public int Width
        {
            get
            {
                int value;
                Internal_GetWidth(mCachedPtr, out value);
                return value;
            }
        }

        public int Height
        {
            get
            {
                int value;
                Internal_GetHeight(mCachedPtr, out value);
                return value;
            }
        }

        public bool GammaCorrection
        {
            get
            {
                bool value;
                Internal_GetGammaCorrection(mCachedPtr, out value);
                return value;
            }
        }

        public int SampleCount
        {
            get
            {
                int value;
                Internal_GetSampleCount(mCachedPtr, out value);
                return value;
            }
        }

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

    // Note: Do not modify IDs as they must match TextureUsage C++ enum
    public enum TextureUsage
    {
        Default = 0x1,
        Dynamic = 0x2,
        Render = 0x200,
        DepthStencil = 0x400,
        LoadStore = 0x800,
        CPUCached = 0x1000
    }
}
