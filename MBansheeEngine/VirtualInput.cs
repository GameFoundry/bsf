using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public static class VirtualInput
    {
        public delegate void OnButtonEventDelegate(VirtualButton btn, int deviceIdx);

        public static event OnButtonEventDelegate OnButtonDown;
        public static event OnButtonEventDelegate OnButtonUp;
        public static event OnButtonEventDelegate OnButtonHeld;

        public static InputConfiguration KeyConfig
        {
            get
            {
                return Internal_GetKeyConfig();
            }

            set
            {
                Internal_SetKeyConfig(value);
            }
        }
		
		public static bool IsButtonDown(VirtualButton button, int deviceIdx = 0)
	    {
            return Internal_IsButtonDown(button, deviceIdx);
	    }

        public static bool IsButtonUp(VirtualButton button, int deviceIdx = 0)
        {
            return Internal_IsButtonUp(button, deviceIdx);
        }

        public static bool IsButtonHeld(VirtualButton button, int deviceIdx = 0)
        {
            return Internal_IsButtonHeld(button, deviceIdx);
        }

        public static float GetAxisValue(VirtualAxis axis, int deviceIdx = 0)
        {
            return Internal_GetAxisValue(axis, deviceIdx);
        }

        private static void Internal_TriggerButtonDown(VirtualButton button, int deviceIdx)
        {
            if (OnButtonDown != null)
                OnButtonDown(button, deviceIdx);
        }

        private static void Internal_TriggerButtonUp(VirtualButton button, int deviceIdx)
        {
            if (OnButtonUp != null)
                OnButtonUp(button, deviceIdx);
        }

        private static void Internal_TriggerButtonHeld(VirtualButton button, int deviceIdx)
        {
            if (OnButtonHeld != null)
                OnButtonHeld(button, deviceIdx);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern InputConfiguration Internal_GetKeyConfig();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetKeyConfig(InputConfiguration inputConfig);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsButtonDown(VirtualButton button, int deviceIdx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsButtonUp(VirtualButton button, int deviceIdx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsButtonHeld(VirtualButton button, int deviceIdx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetAxisValue(VirtualAxis button, int deviceIdx);
	};
}
