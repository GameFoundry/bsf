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
            get { return Internal_GetElapsed(); }
        }

        public static float FrameDelta
        {
            get { return Internal_GetFrameDelta(); }
        }

        public static int FrameNumber
        {
            get { return Internal_GetFrameNumber(); }
        }

        public static UInt64 Precise
        {
            get { return Internal_GetPrecise(); }
        }

        public const float MicroToSecond = 1.0f/1000000.0f;
        public const float SecondToMicro = 1000000.0f;

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetElapsed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetFrameDelta();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetFrameNumber();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt64 Internal_GetPrecise();
    }
}
