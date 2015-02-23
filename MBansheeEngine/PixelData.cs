using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace BansheeEngine
{
    // Do not modify, IDs must match C++ enum PixelFormat
    public enum PixelFormat
    {
        // 8-bit pixel format, all bits red.
        R8 = 1,
        // 2 byte pixel format, 1 byte red, 1 byte green.
        R8G8 = 2,
        // 24-bit pixel format, 8 bits for red, green and blue.
        R8G8B8 = 3,
        // 32-bit pixel format, 8 bits for red, green, blue and alpha.
        R8G8B8A8 = 8,
        // DXT1/BC1 format containing opaque RGB or 1-bit alpha RGB. 4 bits per pixel.
        BC1 = 13,
        // DXT3/BC2 format containing RGB with premultiplied alpha. 4 bits per pixel.
        BC1a = 14,
        // DXT3/BC2 format containing RGB with explicit alpha. 8 bits per pixel.
        BC2 = 15,
        // DXT5/BC2 format containing RGB with explicit alpha. 8 bits per pixel. Better alpha gradients than BC2.
        BC3 = 16,
        // One channel compressed format. 4 bits per pixel.
        BC4 = 17,
        // Two channel compressed format. 8 bits per pixel.
        BC5 = 18,
        // Format storing RGB in half (16-bit) floating point format usable for HDR. 8 bits per pixel.
        BC6H = 19,
        // Format storing RGB with optional alpha channel. Similar to BC1/BC2/BC3 formats but with higher quality and higher decompress overhead. 8 bits per pixel.
        BC7 = 20,
        // 16-bit pixel format, 16 bits (float) for red
        Float16_R = 21,
        // 32-bit, 2-channel s10e5 floating point pixel format, 16-bit red, 16-bit green
        Float16_RG = 22,
        // 48-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue
        Float16_RGB = 23,
        // 64-bit pixel format, 16 bits (float) for red, 16 bits (float) for green, 16 bits (float) for blue, 16 bits (float) for alpha
        Float16_RGBA = 24,
        // 32-bit pixel format, 32 bits (float) for red
        Float32_R = 25,
        // 64-bit, 2-channel floating point pixel format, 32-bit red, 32-bit green
        Float32_RG = 26,
        // 96-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue
        Float32_RGB = 27,
        // 128-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue, 32 bits (float) for alpha
        Float32_RGBA = 28,
        // Depth stencil format, 32bit depth, 8bit stencil + 24 unused
        D32_S8X24 = 29,
        // Depth stencil fomrat, 24bit depth + 8bit stencil
        D24S8 = 30,
        // Depth format, 32bits
        D32 = 31,
        // Depth format, 16bits
        D16 = 32
    };

    public sealed class PixelData : ScriptObject
    {
        public PixelVolume Extents
        {
            get
            {
                PixelVolume volume;
                Internal_GetExtents(mCachedPtr, out volume);
                return volume;
            }
        }

        public PixelFormat Format
        {
            get
            {
                PixelFormat format;
                Internal_GetFormat(mCachedPtr, out format);
                return format;
            }
        }

        public int RawRowPitch
        {
            get
            {
                int rowPitch;
                Internal_GetRowPitch(mCachedPtr, out rowPitch);
                return rowPitch;
            }
        }

        public int RawSlicePitch
        {
            get
            {
                int slicePitch;
                Internal_GetSlicePitch(mCachedPtr, out slicePitch);
                return slicePitch;
            }
        }

        public int RawSize
        {
            get
            {
                int size;
                Internal_GetSize(mCachedPtr, out size);
                return size;
            }
        }

        public bool RawIsConsecutive
        {
            get
            {
                bool isConsecutive;
                Internal_GetIsConsecutive(mCachedPtr, out isConsecutive);
                return isConsecutive;
            }
        }

        // Only for use by native code
        private PixelData()
        { }

        public PixelData(PixelVolume volume, PixelFormat format = PixelFormat.R8G8B8A8)
        {
            Internal_CreateInstance(this, volume, format);
        }

        public PixelData(int width, int height, PixelFormat format = PixelFormat.R8G8B8A8)
        {
            Internal_CreateInstance(this, new PixelVolume(0, 0, width, height), format);
        }

        public PixelData(int width, int height, int depth, PixelFormat format = PixelFormat.R8G8B8A8)
        {
            Internal_CreateInstance(this, new PixelVolume(0, 0, 0, width, height, depth), format);
        }

        public Color GetPixel(int x, int y, int z = 0)
        {
            Color pixel;
            Internal_GetPixel(mCachedPtr, x, y, z, out pixel);
            return pixel;
        }

        public void SetPixel(Color color, int x, int y, int z = 0)
        {
            Internal_SetPixel(mCachedPtr, x, y, z, color);
        }

        public Color[] GetPixels()
        {
            Color[] pixels;
            Internal_GetPixels(mCachedPtr, out pixels);
            return pixels;
        }

        public void SetPixels(Color[] pixels)
        {
            Internal_SetPixels(mCachedPtr, pixels);
        }

        public byte[] GetRawPixels()
        {
            byte[] pixels;
            Internal_GetRawPixels(mCachedPtr, out pixels);
            return pixels;
        }

        public void SetRawPixels(byte[] pixels)
        {
            Internal_SetRawPixels(mCachedPtr, pixels);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(PixelData instance, PixelVolume volume, PixelFormat format);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPixel(IntPtr thisPtr, int x, int y, int z, out Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPixel(IntPtr thisPtr, int x, int y, int z, Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPixels(IntPtr thisPtr, out Color[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPixels(IntPtr thisPtr, Color[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetRawPixels(IntPtr thisPtr, out byte[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRawPixels(IntPtr thisPtr, byte[] value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetExtents(IntPtr thisPtr, out PixelVolume value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetFormat(IntPtr thisPtr, out PixelFormat value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetRowPitch(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetSlicePitch(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetSize(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetIsConsecutive(IntPtr thisPtr, out bool value);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PixelVolume
    {
        private int left, top, right, bottom, front, back;

        public int Left { get { return left; } }
        public int Right { get { return right; } }
        public int Top { get { return top; } }
        public int Bottom { get { return bottom; } }
        public int Front { get { return front; } }
        public int Back { get { return back; } }
        public int Width { get { return right - left; } }
        public int Height { get { return bottom - top; } }
        public int Depth { get { return back - front; } }

        public PixelVolume(int left, int top, int right, int bottom)
        {
            this.left = left;
            this.right = right;
            this.top = top;
            this.bottom = bottom;
            this.front = 0;
            this.back = 1;
        }

        public PixelVolume(int left, int top, int front, int right, int bottom, int back)
        {
            this.left = left;
            this.right = right;
            this.top = top;
            this.bottom = bottom;
            this.front = front;
            this.back = back;
        }
    };
}
