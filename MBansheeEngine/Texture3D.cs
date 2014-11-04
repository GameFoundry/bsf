using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public sealed class Texture3D : Texture
    {
        public int Depth
        {
            get
            {
                int value;
                Internal_GetDepth(mCachedPtr, out value);
                return value;
            }
        }

        // For internal use by the runtime
        private Texture3D()
        { }

        public Texture3D(PixelFormat format, int width, int height, int depth, TextureUsage usage = TextureUsage.Default, 
            bool hasMipmaps = false, bool gammaCorrection = false)
        {
            Internal_CreateInstance(this, format, width, height, depth, usage, hasMipmaps, gammaCorrection);
        }

        public PixelData GetPixels(int mipLevel = 0)
        {
            return Internal_GetPixels(mCachedPtr, mipLevel);
        }

        public void SetPixels(PixelData data, int mipLevel = 0)
        {
            Internal_SetPixels(mCachedPtr, data, mipLevel);
        }

        public AsyncOp GetGPUPixels(int mipLevel = 0)
        {
            return Internal_GetGPUPixels(mCachedPtr, mipLevel);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(Texture3D instance, PixelFormat format, int width,
            int height, int depth, TextureUsage usage, bool hasMipmaps, bool gammaCorrection);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetDepth(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern PixelData Internal_GetPixels(IntPtr thisPtr, int mipLevel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern AsyncOp Internal_GetGPUPixels(IntPtr thisPtr, int mipLevel);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPixels(IntPtr thisPtr, PixelData data, int mipLevel);
    }
}
