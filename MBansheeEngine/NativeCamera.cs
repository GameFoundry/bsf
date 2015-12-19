using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    /// <summary>
    /// Projection type to use by the camera.
    /// </summary>
    public enum ProjectionType // Note: Must match C++ enum ProjectionType
    {
        /// <summary>
        /// Projection type where object size remains constant and parallel lines remain parallel.
        /// </summary>
        Orthographic, 

        /// <summary>
        /// Projection type that emulates human vision. Objects farther away appear smaller.
        /// </summary>
        Perspective
    }

    /// <summary>
    /// Flags that specify which portions, if any, of the viewport are to be cleared before rendering.
    /// </summary>
    [Flags]
    public enum ClearFlags // Don't reorder, used by native code
    {
        None = 0, Color = 0x01, Depth = 0x02, Stencil = 0x04
    }

    /// <summary>
    /// Wrapper around the native Camera class.
    /// <see cref="Camera"/>
    /// </summary>
    internal class NativeCamera : ScriptObject
    {
        internal float aspectRatio
        {
            get { return Internal_GetAspect(mCachedPtr); }
            set { Internal_SetAspect(mCachedPtr, value); }
        }

        internal float nearClipPlane
        {
            get { return Internal_GetNearClip(mCachedPtr); }
            set { Internal_SetNearClip(mCachedPtr, value); }
        }

        internal float farClipPlane
        {
            get { return Internal_GetFarClip(mCachedPtr); }
            set { Internal_SetFarClip(mCachedPtr, value); }
        }

        internal Degree fieldOfView
        {
            get { Degree value; Internal_GetFieldOfView(mCachedPtr, out value); return value; }
            set { Internal_SetFieldOfView(mCachedPtr, ref value); }
        }

        internal Rect2 viewportRect
        {
            get
            {
                Rect2 value;
                Internal_GetViewportRect(mCachedPtr, out value);
                return value;
            }
            set { Internal_SetViewportRect(mCachedPtr, ref value); }
        }

        internal ProjectionType projectionType
        {
            get { return Internal_GetProjectionType(mCachedPtr); }
            set { Internal_SetProjectionType(mCachedPtr, value); }
        }

        internal float orthoHeight
        {
            get { return Internal_GetOrthographicHeight(mCachedPtr); }
            set { Internal_SetOrthographicHeight(mCachedPtr, value); }
        }

        internal float orthoWidth
        {
            get { return Internal_GetOrthographicWidth(mCachedPtr); }
        }

        internal Color clearColor
        {
            get
            {
                Color value;
                Internal_GetClearColor(mCachedPtr, out value);
                return value;
            }
            set { Internal_SetClearColor(mCachedPtr, ref value); }
        }

        internal float clearDepth
        {
            get { return Internal_GetDepthClearValue(mCachedPtr); }
            set { Internal_SetDepthClearValue(mCachedPtr, value); }
        }

        internal UInt16 clearStencil
        {
            get { return Internal_GetStencilClearValue(mCachedPtr); }
            set { Internal_SetStencilClearValue(mCachedPtr, value); }
        }

        internal ClearFlags clearFlags
        {
            get { return Internal_GetClearFlags(mCachedPtr); }
            set { Internal_SetClearFlags(mCachedPtr, value); }
        }

        internal int priority
        {
            get { return Internal_GetPriority(mCachedPtr); }
            set { Internal_SetPriority(mCachedPtr, value); }
        }

        internal UInt64 layers
        {
            get { return Internal_GetLayers(mCachedPtr); }
            set { Internal_SetLayers(mCachedPtr, value); }
        }

        internal Matrix4 projMatrix
        {
            get
            {
                Matrix4 value;
                Internal_GetProjMatrix(mCachedPtr, out value);
                return value;
            }
        }

        internal Matrix4 projMatrixInv
        {
            get
            {
                Matrix4 value;
                Internal_GetProjMatrixInv(mCachedPtr, out value);
                return value;
            }
        }

        internal Matrix4 viewMatrix
        {
            get
            {
                Matrix4 value;
                Internal_GetViewMatrix(mCachedPtr, out value);
                return value;
            }
        }

        internal Matrix4 viewMatrixInv
        {
            get
            {
                Matrix4 value;
                Internal_GetViewMatrixInv(mCachedPtr, out value);
                return value;
            }
        }

        internal int widthPixels
        {
            get { return Internal_GetWidthPixels(mCachedPtr); }
        }

        internal int heightPixels
        {
            get { return Internal_GetHeightPixels(mCachedPtr); }
        }

        internal RenderTarget target
        {
            get
            {
                return _target;
            }
            set
            {
                _target = value;

                IntPtr targetPtr = IntPtr.Zero;
                if (_target != null)
                    targetPtr = _target.mCachedPtr;

                Internal_SetRenderTarget(mCachedPtr, targetPtr);
            }
        }

        internal bool main
        {
            get { return Internal_GetMain(mCachedPtr); }
            set { Internal_SetMain(mCachedPtr, value); }
        }

        internal Vector2I WorldToScreen(Vector3 value)
        {
            Vector2I output;
            Internal_WorldToScreen(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector2 WorldToClip(Vector3 value)
        {
            Vector2 output;
            Internal_WorldToClip(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector3 WorldToView(Vector3 value)
        {
            Vector3 output;
            Internal_WorldToView(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector3 ScreenToWorld(Vector2I value, float depth)
        {
            Vector3 output;
            Internal_ScreenToWorld(mCachedPtr, ref value, depth, out output);
            return output;
        }

        internal Vector3 ScreenToView(Vector2I value, float depth)
        {
            Vector3 output;
            Internal_ScreenToView(mCachedPtr, ref value, depth, out output);
            return output;
        }

        internal Vector2 ScreenToClip(Vector2I value)
        {
            Vector2 output;
            Internal_ScreenToClip(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector3 ViewToWorld(Vector3 value)
        {
            Vector3 output;
            Internal_ViewToWorld(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector2I ViewToScreen(Vector3 value)
        {
            Vector2I output;
            Internal_ViewToScreen(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector2 ViewToClip(Vector3 value)
        {
            Vector2 output;
            Internal_ViewToClip(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector3 ClipToWorld(Vector2 value, float depth)
        {
            Vector3 output;
            Internal_ClipToWorld(mCachedPtr, ref value, depth, out output);
            return output;
        }

        internal Vector3 ClipToView(Vector2 value, float depth)
        {
            Vector3 output;
            Internal_ClipToView(mCachedPtr, ref value, depth, out output);
            return output;
        }

        internal Vector2I ClipToScreen(Vector2 value)
        {
            Vector2I output;
            Internal_ClipToScreen(mCachedPtr, ref value, out output);
            return output;
        }

        internal Ray ScreenToWorldRay(Vector2I value)
        {
            Ray output;
            Internal_ScreenToWorldRay(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector3 ProjectPoint(Vector3 value)
        {
            Vector3 output;
            Internal_ProjectPoint(mCachedPtr, ref value, out output);
            return output;
        }

        internal Vector3 UnprojectPoint(Vector3 value)
        {
            Vector3 output;
            Internal_UnprojectPoint(mCachedPtr, ref value, out output);
            return output;
        }

        private RenderTarget _target;

        public NativeCamera(SceneObject sceneObject)
        {
            IntPtr sceneObjPtr = IntPtr.Zero;
            if (sceneObject != null)
                sceneObjPtr = sceneObject.GetCachedPtr();

            Internal_Create(this, sceneObjPtr);
        }

        internal void UpdateView(SceneObject sceneObject)
        {
            Internal_UpdateView(mCachedPtr, sceneObject.mCachedPtr);
        }

        internal void OnDestroy()
        {
            Internal_OnDestroy(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(NativeCamera instance, IntPtr parentSO);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetAspect(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetAspect(IntPtr instance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetNearClip(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetNearClip(IntPtr instance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetFarClip(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFarClip(IntPtr instance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetFieldOfView(IntPtr instance, out Degree value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFieldOfView(IntPtr instance, ref Degree value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetViewportRect(IntPtr instance, out Rect2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetViewportRect(IntPtr instance, ref Rect2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ProjectionType Internal_GetProjectionType(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetProjectionType(IntPtr instance, ProjectionType value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetOrthographicHeight(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetOrthographicHeight(IntPtr instance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetOrthographicWidth(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetClearColor(IntPtr instance, out Color value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetClearColor(IntPtr instance, ref Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetDepthClearValue(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetDepthClearValue(IntPtr instance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt16 Internal_GetStencilClearValue(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetStencilClearValue(IntPtr instance, UInt16 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern ClearFlags Internal_GetClearFlags(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetClearFlags(IntPtr instance, ClearFlags value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetPriority(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPriority(IntPtr instance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt64 Internal_GetLayers(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetLayers(IntPtr instance, UInt64 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetProjMatrix(IntPtr instance, out Matrix4 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetProjMatrixInv(IntPtr instance, out Matrix4 output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetViewMatrix(IntPtr instance, out Matrix4 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetViewMatrixInv(IntPtr instance, out Matrix4 output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetWidthPixels(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHeightPixels(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_WorldToScreen(IntPtr instance, ref Vector3 value, out Vector2I output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_WorldToClip(IntPtr instance, ref Vector3 value, out Vector2 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_WorldToView(IntPtr instance, ref Vector3 value, out Vector3 output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ScreenToWorld(IntPtr instance, ref Vector2I value, float depth, out Vector3 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ScreenToView(IntPtr instance, ref Vector2I value, float depth, out Vector3 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ScreenToClip(IntPtr instance, ref Vector2I value, out Vector2 output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ViewToWorld(IntPtr instance, ref Vector3 value, out Vector3 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ViewToScreen(IntPtr instance, ref Vector3 value, out Vector2I output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ViewToClip(IntPtr instance, ref Vector3 value, out Vector2 output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClipToWorld(IntPtr instance, ref Vector2 value, float depth, out Vector3 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClipToView(IntPtr instance, ref Vector2 value, float depth, out Vector3 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClipToScreen(IntPtr instance, ref Vector2 value, out Vector2I output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ScreenToWorldRay(IntPtr instance, ref Vector2I value, out Ray output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ProjectPoint(IntPtr instance, ref Vector3 value, out Vector3 output);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UnprojectPoint(IntPtr instance, ref Vector3 value, out Vector3 output);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRenderTarget(IntPtr instance, IntPtr rt);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetMain(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMain(IntPtr instance, bool main);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UpdateView(IntPtr instance, IntPtr parentSO);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_OnDestroy(IntPtr thisPtr);
    }
}
