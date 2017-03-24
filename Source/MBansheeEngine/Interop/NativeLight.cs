//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /** @cond INTEROP */

    /// <summary>
    /// Light type that determines how is light information parsed by the renderer and other systems.
    /// </summary>
    public enum LightType // Note: Must match C++ enum LightType
    {
        Directional, Radial, Spot
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
            get
            {
                Color color;
                Internal_GetColor(mCachedPtr, out color);
                return color;
            }
            set { Internal_SetColor(mCachedPtr, value); }
        }

        internal float AttenuationRadius
        {
            get { return Internal_GetAttenuationRadius(mCachedPtr); }
            set { Internal_SetAttenuationRadius(mCachedPtr, value); }
        }

        internal float SourceRadius
        {
            get { return Internal_GetSourceRadius(mCachedPtr); }
            set { Internal_SetSourceRadius(mCachedPtr, value); }
        }

        internal float Intensity
        {
            get { return Internal_GetIntensity(mCachedPtr); }
            set { Internal_SetIntensity(mCachedPtr, value); }
        }

        internal Degree SpotAngle
        {
            get { return (Degree)Internal_GetSpotAngle(mCachedPtr); }
            set { Internal_SetSpotAngle(mCachedPtr, value.Degrees); }
        }

        internal Degree SpotFalloffAngle
        {
            get { return (Degree)Internal_GetSpotFalloffAngle(mCachedPtr); }
            set { Internal_SetSpotFalloffAngle(mCachedPtr, value.Degrees); }
        }

        internal bool CastsShadow
        {
            get { return Internal_GetCastsShadow(mCachedPtr); }
            set { Internal_SetCastsShadow(mCachedPtr, value); }
        }

        internal bool UseAutoAttenuation
        {
            get { return Internal_GetUseAutoAttenuation(mCachedPtr); }
            set { Internal_SetUseAutoAttenuation(mCachedPtr, value); }
        }

        internal Sphere Bounds
        {
            get
            {
                Sphere bounds;
                Internal_GetBounds(mCachedPtr, out bounds);
                return bounds;
            }
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
        private static extern float Internal_GetAttenuationRadius(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetAttenuationRadius(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSourceRadius(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSourceRadius(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetIntensity(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetIntensity(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetColor(IntPtr instance, out Color color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetColor(IntPtr instance, Color value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetCastsShadow(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCastsShadow(IntPtr instance, bool value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetUseAutoAttenuation(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetUseAutoAttenuation(IntPtr instance, bool value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSpotAngle(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSpotAngle(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetSpotFalloffAngle(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSpotFalloffAngle(IntPtr instance, float value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetBounds(IntPtr instance, out Sphere bounds);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UpdateTransform(IntPtr instance, IntPtr parentSO);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_OnDestroy(IntPtr thisPtr);
    }

    /** @endcond */
}
