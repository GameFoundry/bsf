using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public static class Time
    {
        public static float Elapsed
        {
            get
            {
                return Internal_GetElapsed();
            }
        }

        public static float FrameDelta
        {
            get
            {
                return Internal_GetFrameDelta();
            }
        }

        public static int FrameNumber
        {
            get
            {
                return Internal_GetFrameNumber();
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetElapsed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetFrameDelta();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetFrameNumber();
    }
}
