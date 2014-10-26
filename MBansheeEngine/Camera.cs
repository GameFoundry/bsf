using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public enum ProjectionType
    {
        Orthographic, Perspective
    }

    [Flags]
    public enum ClearFlags
    {
        Color = 0x01, Depth = 0x02, Stencil = 0x04
    }

    public class Camera : Component
    {
        public float aspectRatio
        {
            get { return Internal_GetAspect(mCachedPtr); }
            set { Internal_SetAspect(mCachedPtr, value); }
        }

        public float nearClipPlane
        {
            get { return Internal_GetNearClip(mCachedPtr); }
            set { Internal_SetNearClip(mCachedPtr, value); }
        }

        public float farClipPlane
        {
            get { return Internal_GetFarClip(mCachedPtr); }
            set { Internal_SetFarClip(mCachedPtr, value); }
        }

        public Degree fieldOfView
        {
            get { return Internal_GetFieldOfView(mCachedPtr); }
            set { Internal_SetFieldOfView(mCachedPtr, value); }
        }

        public Rect2 viewportRect
        {
            get { return Internal_GetViewportRect(mCachedPtr); }
            set { Internal_SetViewportRect(mCachedPtr, value); }
        }

        public ProjectionType projectionType
        {
            get { return Internal_GetProjectionType(mCachedPtr); }
            set { Internal_SetProjectionType(mCachedPtr, value); }
        }

        public float orthoHeight
        {
            get { return Internal_GetOrthographicHeight(mCachedPtr); }
            set { Internal_SetOrthographicHeight(mCachedPtr, value); }
        }

        public float orthoWidth
        {
            get { return Internal_GetOrthographicWidth(mCachedPtr); }
        }

        public Color clearColor
        {
            get { return Internal_GetClearColor(mCachedPtr); }
            set { Internal_SetClearColor(mCachedPtr, value); }
        }

        public int clearDepth
        {
            get { return Internal_GetDepthClearValue(mCachedPtr); }
            set { Internal_SetDepthClearValue(mCachedPtr, value); }
        }

        public byte clearStencil
        {
            get { return Internal_GetStencilClearValue(mCachedPtr); }
            set { Internal_SetStencilClearValue(mCachedPtr, value); }
        }

        public ClearFlags clearFlags
        {
            get { return Internal_GetClearFlags(mCachedPtr); }
            set { Internal_SetClearFlags(mCachedPtr, value); }
        }

        public int priority
        {
            get { return Internal_GetPriority(mCachedPtr); }
            set { Internal_SetPriority(mCachedPtr, value); }
        }

        public UInt64 layers
        {
            get { return Internal_GetLayers(mCachedPtr); }
            set { Internal_SetLayers(mCachedPtr, value); }
        }

        public Matrix4 projMatrix
        {
            get { return Internal_GetProjMatrix(mCachedPtr); }
        }

        public Matrix4 projMatrixInv
        {
            get { return Internal_GetProjMatrixInv(mCachedPtr); }
        }

        public Matrix4 viewMatrix
        {
            get { return Internal_GetViewMatrix(mCachedPtr); }
        }

        public Matrix4 viewMatrixInv
        {
            get { return Internal_GetViewMatrixInv(mCachedPtr); }
        }

        public int widthPixels
        {
            get { return Internal_GetWidthPixels(mCachedPtr); }
        }

        public int heightPixels
        {
            get { return Internal_GetHeightPixels(mCachedPtr); }
        }

        public Vector2I WorldToScreen(Vector3 value) { return Internal_WorldToScreen(mCachedPtr, value); }
        public Vector2 WorldToClip(Vector3 value) { return Internal_WorldToClip(mCachedPtr, value); }
        public Vector3 WorldToView(Vector3 value) { return Internal_WorldToView(mCachedPtr, value); }

        public Vector3 ScreenToWorld(Vector2I value) { return Internal_ScreenToWorld(mCachedPtr, value); }
        public Vector3 ScreenToView(Vector2I value) { return Internal_ScreenToView(mCachedPtr, value); }
        public Vector2 ScreenToClip(Vector2I value) { return Internal_ScreenToClip(mCachedPtr, value); }

        public Vector3 ViewToWorld(Vector3 value) { return Internal_ViewToWorld(mCachedPtr, value); }
        public Vector2I ViewToScreen(Vector3 value) { return Internal_ViewToScreen(mCachedPtr, value); }
        public Vector2 ViewToClip(Vector3 value) { return Internal_ViewToClip(mCachedPtr, value); }

        public Vector3 ClipToWorld(Vector2 value) { return Internal_ClipToWorld(mCachedPtr, value); }
        public Vector3 ClipToView(Vector2 value) { return Internal_ClipToView(mCachedPtr, value); }
        public Vector2I ClipToScreen(Vector2 value) { return Internal_ClipToScreen(mCachedPtr, value); }

        public Ray ScreenToWorldRay(Vector2I value) { return Internal_ScreenToWorldRay(mCachedPtr, value); }
        public Vector3 ProjectPoint(Vector3 value) { return Internal_ProjectPoint(mCachedPtr, value); }
        public Vector3 UnprojectPoint(Vector3 value) { return Internal_UnprojectPoint(mCachedPtr, value); }

        // TODO - Add RenderTexture

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
        private static extern Degree Internal_GetFieldOfView(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFieldOfView(IntPtr instance, Degree value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Rect2 Internal_GetViewportRect(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetViewportRect(IntPtr instance, Rect2 value);

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
        private static extern Color Internal_GetClearColor(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetClearColor(IntPtr instance, Color value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetDepthClearValue(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetDepthClearValue(IntPtr instance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern byte Internal_GetStencilClearValue(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetStencilClearValue(IntPtr instance, byte value);

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
        private static extern Matrix4 Internal_GetProjMatrix(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Matrix4 Internal_GetProjMatrixInv(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Matrix4 Internal_GetViewMatrix(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Matrix4 Internal_GetViewMatrixInv(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetWidthPixels(IntPtr instance);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHeightPixels(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2I Internal_WorldToScreen(IntPtr instance, Vector3 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2 Internal_WorldToClip(IntPtr instance, Vector3 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_WorldToView(IntPtr instance, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_ScreenToWorld(IntPtr instance, Vector2I value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_ScreenToView(IntPtr instance, Vector2I value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2 Internal_ScreenToClip(IntPtr instance, Vector2I value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_ViewToWorld(IntPtr instance, Vector3 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2I Internal_ViewToScreen(IntPtr instance, Vector3 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2 Internal_ViewToClip(IntPtr instance, Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_ClipToWorld(IntPtr instance, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_ClipToView(IntPtr instance, Vector2 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector2I Internal_ClipToScreen(IntPtr instance, Vector2 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Ray Internal_ScreenToWorldRay(IntPtr instance, Vector2I value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_ProjectPoint(IntPtr instance, Vector3 value);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 Internal_UnprojectPoint(IntPtr instance, Vector3 value);
    }
}
