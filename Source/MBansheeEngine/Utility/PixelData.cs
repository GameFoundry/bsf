//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup Utility
     *  @{
     */

    /// <summary>
    /// Pixel formats usable by images, textures and render surfaces.
    /// </summary>
    public enum PixelFormat // Note: Must match C++ enum PixelFormat
    {
        /// <summary>
        /// 8-bit pixel format, all bits red.
        /// </summary>
        R8 = 1,
        /// <summary>
        /// 2 byte pixel format, 1 byte red, 1 byte green.
        /// </summary>
        R8G8 = 2,
        /// <summary>
        /// 24-bit pixel format, 8 bits for red, green and blue.
        /// </summary>
        R8G8B8 = 3,
        /// <summary>
        /// 32-bit pixel format, 8 bits for red, green, blue and alpha.
        /// </summary>
        R8G8B8A8 = 8,
        /// <summary>
        /// DXT1/BC1 format containing opaque RGB or 1-bit alpha RGB. 4 bits per pixel.
        /// </summary>
        BC1 = 13,
        /// <summary>
        /// DXT3/BC2 format containing RGB with premultiplied alpha. 4 bits per pixel.
        /// </summary>
        BC1a = 14,
        /// <summary>
        /// DXT3/BC2 format containing RGB with explicit alpha. 8 bits per pixel.
        /// </summary>
        BC2 = 15,
        /// <summary>
        /// DXT5/BC2 format containing RGB with explicit alpha. 8 bits per pixel. Better alpha gradients than BC2.
        /// </summary>
        BC3 = 16,
        /// <summary>
        /// One channel compressed format. 4 bits per pixel.
        /// </summary>
        BC4 = 17,
        /// <summary>
        /// Two channel compressed format. 8 bits per pixel.
        /// </summary>
        BC5 = 18,
        /// <summary>
        /// Format storing RGB in half (16-bit) floating point format usable for HDR. 8 bits per pixel.
        /// </summary>
        BC6H = 19,
        /// <summary>
        /// Format storing RGB with optional alpha channel. Similar to BC1/BC2/BC3 formats but with higher quality and 
        /// higher decompress overhead. 8 bits per pixel.
        /// </summary>
        BC7 = 20,
        /// <summary>
        /// 16-bit 1-channel s10e5 floating point pixel format, 16 bits (float) for red.
        /// </summary>
        Float16_R = 21,
        /// <summary>
        /// 32-bit, 2-channel s10e5 floating point pixel format, 16-bits (float) for red, 16-bits (float) for green.
        /// </summary>
        Float16_RG = 22,
        /// <summary>
        /// 48-bit, 3-channel s10e5 floating point pixel format,, 16 bits (float) for red, 16 bits (float) for green, 
        /// 16 bits (float) for blue.
        /// </summary>
        Float16_RGB = 23,
        /// <summary>
        /// 64-bit, 4-channel s10e5 floating point pixel format, 16 bits (float) for red, 16 bits (float) for green, 
        /// 16 bits (float) for blue, 16 bits (float) for alpha.
        /// </summary>
        Float16_RGBA = 24,
        /// <summary>
        /// 32-bit pixel format, 32 bits (float) for red.
        /// </summary>
        Float32_R = 25,
        /// <summary>
        /// 64-bit, pixel format, 32-bits (float) for red, 32-bits (float) for green.
        /// </summary>
        Float32_RG = 26,
        /// <summary>
        /// 96-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue.
        /// </summary>
        Float32_RGB = 27,
        /// <summary>
        /// 128-bit pixel format, 32 bits (float) for red, 32 bits (float) for green, 32 bits (float) for blue, 
        /// 32 bits (float) for alpha.
        /// </summary>
        Float32_RGBA = 28,
        /// <summary>
        /// Depth stencil format, 32bit depth, 8bit stencil + 24 unused.
        /// </summary>
        D32_S8X24 = 29,
        /// <summary>
        /// Depth stencil format, 24bit depth + 8bit stencil.
        /// </summary>
        D24S8 = 30,
        /// <summary>
        /// Depth format, 32bits.
        /// </summary>
        D32 = 31,
        /// <summary>
        /// Depth format, 16bits.
        /// </summary>
        D16 = 32
    };

    /// <summary>
    /// A buffer describing a volume (3D), image (2D) or line (1D) of pixels in memory. Pixels are stored as a succession 
    /// of "depth" slices, each containing "height" rows of "width" pixels.
    /// </summary>
    public sealed class PixelData : ScriptObject
    {
        /// <summary>
        /// Width, height and depth of the pixels this object is capable of holding.
        /// </summary>
        public PixelVolume Extents
        {
            get
            {
                PixelVolume volume;
                Internal_GetExtents(mCachedPtr, out volume);
                return volume;
            }
        }

        /// <summary>
        /// Format of the pixels in the buffer.
        /// </summary>
        public PixelFormat Format
        {
            get
            {
                PixelFormat format;
                Internal_GetFormat(mCachedPtr, out format);
                return format;
            }
        }

        /// <summary>
        /// Returns number of bytes per a row of pixels.
        /// </summary>
        public int RawRowPitch
        {
            get
            {
                int rowPitch;
                Internal_GetRowPitch(mCachedPtr, out rowPitch);
                return rowPitch;
            }
        }

        /// <summary>
        /// Returns number of bytes per a 2D slice/plane of pixels.
        /// </summary>
        public int RawSlicePitch
        {
            get
            {
                int slicePitch;
                Internal_GetSlicePitch(mCachedPtr, out slicePitch);
                return slicePitch;
            }
        }

        /// <summary>
        /// Returns total number of bytes used by all the pixels.
        /// </summary>
        public int RawSize
        {
            get
            {
                int size;
                Internal_GetSize(mCachedPtr, out size);
                return size;
            }
        }

        /// <summary>
        /// Checks are the pixels in the buffer consecutive. If this is false then the buffer has padding and you should
        /// check <see cref="RawRowPitch"/> and <see cref="RawSlicePitch"/> when accessing it directly.
        /// </summary>
        public bool RawIsConsecutive
        {
            get
            {
                bool isConsecutive;
                Internal_GetIsConsecutive(mCachedPtr, out isConsecutive);
                return isConsecutive;
            }
        }

        /// <summary>
        /// Constructor for internal use by the runtime.
        /// </summary>
        private PixelData()
        { }

        /// <summary>
        /// Creates a new pixel data buffer capable of storing the specified amount of pixels.
        /// </summary>
        /// <param name="volume">Width, height and depth determining number of pixels to store.</param>
        /// <param name="format">Format of individual pixels.</param>
        public PixelData(PixelVolume volume, PixelFormat format = PixelFormat.R8G8B8A8)
        {
            Internal_CreateInstance(this, ref volume, format);
        }

        /// <summary>
        /// Creates a new 2D pixel data buffer capable of storing the specified amount of pixels.
        /// </summary>
        /// <param name="width">Number of pixels in each row.</param>
        /// <param name="height">Number of pixels in each column.</param>
        /// <param name="format">Format of individual pixels.</param>
        public PixelData(int width, int height, PixelFormat format = PixelFormat.R8G8B8A8)
        {
            PixelVolume volume = new PixelVolume(0, 0, width, height);

            Internal_CreateInstance(this, ref volume, format);
        }

        /// <summary>
        /// Creates a new 3D pixel data buffer capable of storing the specified amount of pixels.
        /// </summary>
        /// <param name="width">Number of pixels in each row.</param>
        /// <param name="height">Number of pixels in each column.</param>
        /// <param name="depth">Number of 2D slices.</param>
        /// <param name="format">Format of individual pixels.</param>
        public PixelData(int width, int height, int depth, PixelFormat format = PixelFormat.R8G8B8A8)
        {
            PixelVolume volume = new PixelVolume(0, 0, 0, width, height, depth);

            Internal_CreateInstance(this, ref volume, format);
        }

        /// <summary>
        /// Returns a pixel at the specified location in the buffer.
        /// </summary>
        /// <param name="x">X coordinate of the pixel.</param>
        /// <param name="y">Y coordinate of the pixel.</param>
        /// <param name="z">Z coordinate of the pixel.</param>
        /// <returns>Value of the pixel, or undefined value if coordinates are out of range.</returns>
        public Color GetPixel(int x, int y, int z = 0)
        {
            Color pixel;
            Internal_GetPixel(mCachedPtr, x, y, z, out pixel);
            return pixel;
        }

        /// <summary>
        /// Sets a pixel at the specified location in the buffer.
        /// </summary>
        /// <param name="color">Color of the pixel to set.</param>
        /// <param name="x">X coordinate of the pixel.</param>
        /// <param name="y">Y coordinate of the pixel.</param>
        /// <param name="z">Z coordinate of the pixel.</param>
        public void SetPixel(Color color, int x, int y, int z = 0)
        {
            Internal_SetPixel(mCachedPtr, x, y, z, ref color);
        }

        /// <summary>
        /// Returns values of all pixels. 
        /// </summary>
        /// <returns>All pixels in the buffer ordered consecutively. Pixels are stored as a succession of "depth" slices, 
        ///          each containing "height" rows of "width" pixels.</returns>
        public Color[] GetPixels()
        {
            Color[] pixels;
            Internal_GetPixels(mCachedPtr, out pixels);
            return pixels;
        }

        /// <summary>
        /// Sets all pixels in the buffer. Caller must ensure that number of pixels match the extends of the buffer.
        /// </summary>
        /// <param name="pixels">All pixels to set ordered consecutively. Pixels are stored as a succession of "depth" 
        ///                      slices, each containing "height" rows of "width" pixels.</param>
        public void SetPixels(Color[] pixels)
        {
            Internal_SetPixels(mCachedPtr, pixels);
        }

        /// <summary>
        /// Returns all pixels in the buffer as raw bytes.
        /// </summary>
        /// <returns>Raw pixel bytes. It is up to the caller to interpret the pixel format and account for potential 
        ///          row and slice pitch values.</returns>
        public byte[] GetRawPixels()
        {
            byte[] pixels;
            Internal_GetRawPixels(mCachedPtr, out pixels);
            return pixels;
        }

        /// <summary>
        /// Sets all pixels in the buffer as raw bytes.
        /// </summary>
        /// <param name="pixels">Raw pixel bytes. It is up to the caller to set the proper pixel format and account for 
        ///                      potential row and slice pitch values.</param>
        public void SetRawPixels(byte[] pixels)
        {
            Internal_SetRawPixels(mCachedPtr, pixels);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(PixelData instance, ref PixelVolume volume, PixelFormat format);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPixel(IntPtr thisPtr, int x, int y, int z, out Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPixel(IntPtr thisPtr, int x, int y, int z, ref Color value);

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

    /// <summary>
    /// Represents a 3D region of pixels used for referencing pixel data.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct PixelVolume // Note: Must match C++ class PixelVolume
    {
        private int left, top, right, bottom, front, back;

        /// <summary>
        /// Returns the left border of the pixel region (minimal X).
        /// </summary>
        public int Left { get { return left; } }

        /// <summary>
        /// Returns the right border of the pixel region (maximal X).
        /// </summary>
        public int Right { get { return right; } }

        /// <summary>
        /// Returns the top border of the pixel region (minimal Y).
        /// </summary>
        public int Top { get { return top; } }

        /// <summary>
        /// Returns the bottom border of the pixel region (maximal Y).
        /// </summary>
        public int Bottom { get { return bottom; } }

        /// <summary>
        /// Returns the front border of the pixel region (minimal Z).
        /// </summary>
        public int Front { get { return front; } }

        /// <summary>
        /// Returns the back border of the pixel region (maximal Z).
        /// </summary>
        public int Back { get { return back; } }

        /// <summary>
        /// Returns the number of pixels between right and left borders of the volume.
        /// </summary>
        public int Width { get { return right - left; } }

        /// <summary>
        /// Returns the number of pixels between bottom and top borders of the volume.
        /// </summary>
        public int Height { get { return bottom - top; } }

        /// <summary>
        /// Returns the number of pixels between back and front borders of the volume.
        /// </summary>
        public int Depth { get { return back - front; } }

        /// <summary>
        /// Creates a new 2D region.
        /// </summary>
        /// <param name="left">Left border of the region.</param>
        /// <param name="top">Top border of the region.</param>
        /// <param name="right">Right border of the region. Must be larger than left border.</param>
        /// <param name="bottom">Bottom border of the region. Must be larger than top border.</param>
        public PixelVolume(int left, int top, int right, int bottom)
        {
            this.left = left;
            this.right = right;
            this.top = top;
            this.bottom = bottom;
            this.front = 0;
            this.back = 1;
        }

        /// <summary>
        /// Creates a new 3D region.
        /// </summary>
        /// <param name="left">Left border of the region.</param>
        /// <param name="top">Top border of the region.</param>
        /// <param name="front">Front border of the region.</param>
        /// <param name="right">Right border of the region. Must be larger than left border.</param>
        /// <param name="bottom">Bottom border of the region. Must be larger than top border.</param>
        /// <param name="back">Back border of the region. Must be larger than back border.</param>
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

    /** @} */
}
