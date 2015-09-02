using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Light type that determines how is light information parsed by the renderer and other systems.
    /// </summary>
    public enum LightType // Note: Must match C++ enum LightType
    {
        Directional, Point, Spot
    }

    /// <summary>
    /// Wrapper around the native Light class.
    /// <see cref="Light"/>
    /// </summary>
    internal class NativeLight : ScriptObject
    {
        internal LightType Type
        {
            get { return Internal_GetType(mCachedPtr); }
            set { Internal_SetType(mCachedPtr, value); }
        }

        internal Color Color
        {
            get { return Internal_GetColor(mCachedPtr); }
            set { Internal_SetColor(mCachedPtr, value); }
        }

        internal float Range
        {
            get { return Internal_GetRange(mCachedPtr); }
            set { Internal_SetRange(mCachedPtr, value); }
        }

        internal float Intensity
        {
            get { return Internal_GetIntensity(mCachedPtr); }
            set { Internal_SetIntensity(mCachedPtr, value); }
        }

        internal Degree SpotAngle
        {
            get { return Internal_GetSpotAngle(mCachedPtr); }
            set { Internal_SetSpotAngle(mCachedPtr, value.Degrees); }
        }

        internal Degree SpotFalloffAngle
        {
            get { return Internal_GetSpotFalloffAngle(mCachedPtr); }
            set { Internal_SetSpotFalloffAngle(mCachedPtr, value.Degrees); }
        }

        internal bool CastsShadow
        {
            get { return Internal_GetCastsShadow(mCachedPtr); }
            set { Internal_SetCastsShadow(mCachedPtr, value); }
        }

        internal Sphere Bounds
        {
            get { return Internal_GetBounds(mCachedPtr); }
        }

        internal NativeLight(SceneObject sceneObject)
        {
            IntPtr sceneObjPtr = IntPtr.Zero;
            if (sceneObject != null)
                sceneObjPtr = sceneObject.GetCachedPtr();

            Internal_Create(this, sceneObjPtr);
        }

        internal void UpdateTransform(SceneObject parentSO)
        {
            if (parentSO != null)
                Internal_UpdateTransform(mCachedPtr, parentSO.GetCachedPtr());
        }

        internal void OnDestroy()
        {
            Internal_OnDestroy(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(NativeLight instance, IntPtr parentSO);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern LightType Internal_GetType(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetType(IntPtr instance, LightType value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetRange(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRange(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetIntensity(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIntensity(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Color Internal_GetColor(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetColor(IntPtr instance, Color value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetCastsShadow(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCastsShadow(IntPtr instance, bool value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSpotAngle(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSpotAngle(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSpotFalloffAngle(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSpotFalloffAngle(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Sphere Internal_GetBounds(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UpdateTransform(IntPtr instance, IntPtr parentSO);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_OnDestroy(IntPtr thisPtr);
    }
}
