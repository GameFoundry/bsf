using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    // Don't reorder, used by native code
    public enum LightType
    {
        Directional, Point, Light
    }

    internal class LightInternal : ScriptObject
    {
        internal Vector3 Position
        {
            get { return Internal_GetPosition(mCachedPtr); }
            set { Internal_SetPosition(mCachedPtr, value); }
        }

        internal Quaternion Rotation
        {
            get { return Internal_GetRotation(mCachedPtr); }
            set { Internal_SetRotation(mCachedPtr, value); }
        }

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

        internal LightInternal(SceneObject sceneObject)
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
        private static extern void Internal_Create(LightInternal instance, IntPtr parentSO);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_GetPosition(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPosition(IntPtr instance, Vector3 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Quaternion Internal_GetRotation(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRotation(IntPtr instance, Quaternion value);
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
