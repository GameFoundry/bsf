using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public class RenderTarget : ScriptObject
    {
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

        public int Priority
        {
            get
            {
                int value;
                Internal_GetPriority(mCachedPtr, out value);
                return value;
            }

            set
            {
                Internal_SetPriority(mCachedPtr, value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetWidth(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetHeight(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetGammaCorrection(IntPtr thisPtr, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetSampleCount(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPriority(IntPtr thisPtr, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPriority(IntPtr thisPtr, int value);
    }
}